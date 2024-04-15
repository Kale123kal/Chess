#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>

namespace Chess
{
    struct Piece
    {
        Piece(const Piece&) = delete;
        static const int None = 0;
        static const int King = 1;
        static const int Queen = 2;
        static const int Bishop = 3;
        static const int Knight = 4;
        static const int Rook = 5;
        static const int Pawn = 6;

        static const int White = 8;
        static const int Black = 16;

        static const int typeMask = 0b00111;
		static const int blackMask = 0b10000;
		static const int whiteMask = 0b01000;
		static const int colorMask = whiteMask | blackMask;

		static bool IsColor (int piece, int colour) {
			return (piece & colorMask) == colour;
		}

		static int Color (int piece) {
			return (piece & colorMask) == White? 1 : 0;
		}

		static int IsType (int piece, int pieceType) {
			return (piece & typeMask) == pieceType;
		}

        static int PieceType (int piece) {
			return (piece & typeMask)-1;
		}

		// static bool IsRookOrQueen (int piece) {
		// 	return (piece & 0b110) == 0b110;
		// }

		// static bool IsBishopOrQueen (int piece) {
		// 	return (piece & 0b101) == 0b101;
		// }

		static bool IsSlidingPiece (int piece) {
			return ((piece & typeMask) == Queen) || ((piece & typeMask) == Bishop) || ((piece & typeMask) == Rook);
		}
    };

    struct QueuedMove
    {
        int index, piece, preIndex, prePiece;
    };

    struct Moves
    {
        int startSquare, targetSquare;
    };
    

    class Board
    {
    public:
        Board(const Board&) = delete;
        ~Board();

        static Board& Get(){ return s_Instance;}

        int* const GetBoard(){ return Get().board; }
        void removePiece(int);
        void LoadPositionFromFen(std::string fen);
        void undo();
        void move(int index, int piece);
        int legalMove(int index);
        int GetNumOfMoves(){ return numOfQueuedMoves; }
        int GetColorToMove(){ return colorToMove; }
        std::vector<Moves> GetPossibleMoves(int startSquare);
        std::unordered_map<int, int*> numSquaresToEdge;
        void SetPromotTo(int promot);

    private:
        Board();
        void preComputedMoveData();
        void generateMoves();
        void generateSlidingMoves(int startSquare, int piece);
        void generateKMoves(int startSquare, int piece);
        void generatePawnMoves(int startSquare, int piece);
        void promot();
    private:
        static Board s_Instance;
        // const std::string startFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
        const std::string startFen = "r3k2r/Q3bn/8/8/8/8/BPPN/R3K2R";
        int* board;
        std::unordered_map<int, QueuedMove> m_Queue;
        std::unordered_map<int, int*> knightSquaresToEdge;
        std::vector<Moves> moves;
        int numOfQueuedMoves = 0;
        int numOfMoves = 0;
        int directionOffsets[8] = {8, -8, -1, 1, 7, -7, 9, -9};
        int knightDirectionOffsets[8] = {15, -15, 17, -17, 6, -6, 10, -10};
        int colorToMove = 8;
        int friendlyColor = colorToMove;
        int opponentColor = colorToMove == 8? 16: 8;
        bool enPassant = false;
        int enPassantMoveIndex;
        int enPassantTargetSquare;
        int enPassantCaptureIndex;
        bool kingSideCastlingW = true;
        bool queenSideCastlingW = true;
        bool kingSideCastlingB = true;
        bool queenSideCastlingB = true;
    };
}
