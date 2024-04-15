#include "Board.h"

#include "imgui.h"
#include "glm/glm.hpp"

#include <unordered_map>
#include <sstream>
#include <cctype>

namespace Chess
{
    Board Board::s_Instance;

    Board::Board()
    {
        board = new int[64];
        LoadPositionFromFen(startFen);
        preComputedMoveData();
    }

    Board::~Board()
    {
        delete[] board;
    }

    void Board::LoadPositionFromFen(std::string fen)
    {
        std::unordered_map<char, int> pieceTypeFromFen;
        pieceTypeFromFen['k'] = Piece::King , pieceTypeFromFen['q'] = Piece::Queen, pieceTypeFromFen['r'] = Piece::Rook, pieceTypeFromFen['p'] = Piece::Pawn, pieceTypeFromFen['b'] = Piece::Bishop, pieceTypeFromFen['n'] = Piece::Knight;

        std::stringstream ss(fen);
        std::string fenBoard;
        ss >> fenBoard;
        int file = 0, rank = 7;

        for (char symbol : fenBoard)
        {
            if(symbol == '/')
                file = 0, rank--;
            else
            {
                if(isdigit(symbol))
                    file += atoi(&symbol);
                else
                {
                    int pieceColor = isupper(symbol)? Piece::White : Piece::Black;
                    int pieceType = pieceTypeFromFen[tolower(symbol)];
                    board[rank*8 + file] = pieceColor | pieceType;
                    file++;
                }
            }
        }
        
    }

    void Board::move(int index, int piece)
    {
        int legal = legalMove(index);
        if(legal)
        {
            m_Queue[numOfQueuedMoves].preIndex = index;
            m_Queue[numOfQueuedMoves].prePiece = board[index];
            board[index] = piece;

            if(enPassantCaptureIndex == legal)
                board[enPassantTargetSquare] = 0;
            if(Piece::IsType(piece, Piece::King))
            {
                if(index - m_Queue[numOfQueuedMoves].index==2)
                {
                    board[index-1] = Piece::Rook | colorToMove;
                    board[index+1] = 0;
                    Piece::Color(piece)? kingSideCastlingW = false : kingSideCastlingB = false;
                }
                else if(index - m_Queue[numOfQueuedMoves].index==-2)
                {
                    board[index+1] = Piece::Rook | colorToMove;
                    board[index-2] = 0;
                    Piece::Color(piece)? queenSideCastlingW = false : queenSideCastlingB = false;
                }
                else
                {
                    if(Piece::Color(piece))
                        kingSideCastlingW = false, queenSideCastlingW = false;
                    else
                        kingSideCastlingB = false, queenSideCastlingB = false;
                }
            }
            else if(Piece::IsType(piece, Piece::Rook) && m_Queue[numOfQueuedMoves].index == 0)
                queenSideCastlingW = false;
            else if(Piece::IsType(piece, Piece::Rook) && m_Queue[numOfQueuedMoves].index == 7)
                kingSideCastlingW = false;
            else if(Piece::IsType(piece, Piece::Rook) && m_Queue[numOfQueuedMoves].index == 56)
                queenSideCastlingB = false;
            else if(Piece::IsType(piece, Piece::Rook) && m_Queue[numOfQueuedMoves].index == 63)
                kingSideCastlingB = false;

            colorToMove = (numOfQueuedMoves+2)%2? 16 : 8; 
            friendlyColor = colorToMove;
            opponentColor = colorToMove == 8? 16: 8;

                

            enPassant = legal-1==enPassantMoveIndex;
        }
        else
            undo();
        moves.clear();
        enPassantMoveIndex = -1;
        enPassantCaptureIndex = -1;
    }

    int Board::legalMove(int index)
    {   
        int moveIndex = 0;
        if(Piece::IsColor(m_Queue[numOfQueuedMoves].piece, colorToMove))
        {
            for(auto move : moves)
            {
                moveIndex++;
                if(move.startSquare == m_Queue[numOfQueuedMoves].index && move.targetSquare == index )
                    return moveIndex;
            }
        }
        return 0;
    }

    void Board::removePiece(int index)
    {
        numOfQueuedMoves++;
        m_Queue[numOfQueuedMoves] = QueuedMove{index, board[index], -1, -1};
        board[index] = 0;
        generateMoves();
    }

    void Board::undo()
    {
        if(numOfQueuedMoves)
        {
            colorToMove = (numOfQueuedMoves+1)%2? 16 : 8;
            if(m_Queue[numOfQueuedMoves].preIndex != -1)
            {
                int index = m_Queue[numOfQueuedMoves].preIndex;
                int piece = board[index];
                if(Piece::IsType(piece, Piece::King))
                {
                    if(index - m_Queue[numOfQueuedMoves].index==2)
                    {
                        board[index+1] = Piece::Rook | colorToMove;
                        board[index-1] = 0;
                        ((numOfQueuedMoves+2)%2)? kingSideCastlingW = true : kingSideCastlingB = true;
                    }
                    else if(index - m_Queue[numOfQueuedMoves].index==-2)
                    {
                        board[index-2] = Piece::Rook | colorToMove;
                        board[index+1] = 0;
                        ((numOfQueuedMoves+2)%2)? queenSideCastlingW = true : queenSideCastlingB = true;
                    }
                }
                board[m_Queue[numOfQueuedMoves].preIndex] = m_Queue[numOfQueuedMoves].prePiece;

                numOfQueuedMoves--;
            }
            else
                numOfQueuedMoves--;
            colorToMove = (numOfQueuedMoves+2)%2? 16 : 8;
            friendlyColor = colorToMove;
            opponentColor = colorToMove == 8? 16: 8;
            board[m_Queue[numOfQueuedMoves+1].index] = m_Queue[numOfQueuedMoves+1].piece;
            moves.clear();
            enPassantMoveIndex = -1;
        }
    }

    void Board::preComputedMoveData()
    {
        for(int file = 0; file < 8; file++)
        {
            for(int rank = 0; rank < 8; rank++)
            {
                int numNorth = 7 - rank, numSouth = rank;
                int numWest = file, numEast = 7 - file;

                int squareIndex = rank * 8 + file;

                numSquaresToEdge[squareIndex] = new int[8]
                                                {numNorth,
                                                 numSouth,
                                                 numWest,
                                                 numEast,
                                                 std::min(numNorth, numWest),
                                                 std::min(numSouth, numEast),
                                                 std::min(numNorth, numEast),
                                                 std::min(numSouth, numWest)};
                knightSquaresToEdge[squareIndex] = new int[8]
                                                {numNorth>1&&numWest>0,
                                                 numSouth>1&&numEast>0,
                                                 numNorth>1&&numEast>0,
                                                 numSouth>1&&numWest>0,
                                                 numNorth>0&&numWest>1,
                                                 numSouth>0&&numEast>1,
                                                 numNorth>0&&numEast>1,
                                                 numSouth>0&&numWest>1};
            }
        }
    }

    void Board::generateMoves()
    {
        for(int startSquare = 0; startSquare < 64; startSquare++)
        {
            int piece = m_Queue[numOfQueuedMoves].piece;
            if(Piece::IsColor(piece, colorToMove))
            {
                if(Piece::IsSlidingPiece(piece))
                    generateSlidingMoves(startSquare, piece);
                else if(Piece::IsType(piece, Piece::Pawn))
                    generatePawnMoves(startSquare, piece);
                else
                    generateKMoves(startSquare, piece);
            }
        }
    }

    void Board::generateSlidingMoves(int startSquare, int piece)
    {
        int startDirIndex = Piece::IsType(piece, Piece::Bishop)? 4 : 0;
        int endDirIndex = Piece::IsType(piece, Piece::Rook)? 4 : 8;
        for(int directionIndex = startDirIndex; directionIndex < endDirIndex; directionIndex++)
        {
            for(int n = 0; n < numSquaresToEdge[startSquare][directionIndex]; n++)
            {
                int targetSquare = startSquare + directionOffsets[directionIndex] * (n + 1);
                int pieceOnTargetSquare = board[targetSquare];

                if(Piece::IsColor(pieceOnTargetSquare, friendlyColor))
                    break;
                moves.push_back(Moves{startSquare, targetSquare});

                if(Piece::IsColor(pieceOnTargetSquare, opponentColor))
                    break;
            }
        }
    }
    
    void Board::generateKMoves(int startSquare, int piece)
    {
        for(int directionIndex = 0; directionIndex < 8; directionIndex++)
        {
            int targetSquare;
            if(Piece::IsType(piece, Piece::Knight) && knightSquaresToEdge[startSquare][directionIndex])
                targetSquare = startSquare + knightDirectionOffsets[directionIndex];
            else if (Piece::IsType(piece, Piece::King) && numSquaresToEdge[startSquare][directionIndex])
            {
                targetSquare = startSquare + directionOffsets[directionIndex];
                if((Piece::Color(piece))?startSquare == 4:startSquare == 60)
                {
                    if(!board[startSquare+2] && (Piece::Color(piece)? kingSideCastlingW:kingSideCastlingB))
                        moves.push_back(Moves{startSquare, startSquare+2});
                    if(!board[startSquare-2] && !board[startSquare-3] && (Piece::Color(piece)? queenSideCastlingW:queenSideCastlingB))
                        moves.push_back(Moves{startSquare, startSquare-2});
                }
            }
            else
                continue;

            if(targetSquare >= 0 && targetSquare < 64)
            {
                int pieceOnTargetSquare = board[targetSquare];

                if(Piece::IsColor(pieceOnTargetSquare, friendlyColor))
                    continue;
                moves.push_back(Moves{startSquare, targetSquare});

                if(Piece::IsColor(pieceOnTargetSquare, opponentColor))
                    continue;
            }
        }
    }

    void Board::generatePawnMoves(int startSquare, int piece)
    {
        int targetSquare, pieceOnTargetSquare;

         //enPassant
        if(enPassant && (startSquare == enPassantTargetSquare-1 || startSquare == enPassantTargetSquare+1))
        {
            moves.push_back(Moves{startSquare, Piece::Color(piece)?enPassantTargetSquare+8:enPassantTargetSquare-8});
            enPassantCaptureIndex = moves.size();
        }

        //one square pawn push
        targetSquare = Piece::Color(piece)?startSquare + 8 : startSquare - 8;

        pieceOnTargetSquare = board[targetSquare];
        if(!pieceOnTargetSquare)
            moves.push_back(Moves{startSquare, targetSquare});

        //two square pawn push
        if(numSquaresToEdge[startSquare][!Piece::Color(piece)] == 6)
        {

            targetSquare = Piece::Color(piece)?startSquare + 16 : startSquare - 16;

            pieceOnTargetSquare = board[targetSquare];
            if(!pieceOnTargetSquare)
            {
                enPassantMoveIndex = moves.size();
                moves.push_back(Moves{startSquare, targetSquare});
                enPassantTargetSquare = targetSquare;
            }
        }

        //pawn capture square
        targetSquare = Piece::Color(piece)?startSquare + 7 : startSquare - 7;

        pieceOnTargetSquare = board[targetSquare];
        if(Piece::IsColor(pieceOnTargetSquare, opponentColor))
            moves.push_back(Moves{startSquare, targetSquare});

        targetSquare = Piece::Color(piece)?startSquare + 9 : startSquare - 9;

        pieceOnTargetSquare = board[targetSquare];
        if(Piece::IsColor(pieceOnTargetSquare, opponentColor))
            moves.push_back(Moves{startSquare, targetSquare});
    }
    void Board::SetPromotTo(int promot)
    {
        int index = m_Queue[numOfQueuedMoves].preIndex;
        int piece = board[index];
        if(numSquaresToEdge[index][!Piece::Color(piece)]==0)
        {
            board[index] = promot | opponentColor;
        }
    }

    std::vector<Moves> Board::GetPossibleMoves(int startSquare)
    {
        std::vector<Moves> move;
        for(const auto &m : moves)
        {
            if(m.startSquare == startSquare)
                move.push_back(m);
        }
        return move;
    }
}
