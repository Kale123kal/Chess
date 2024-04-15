#include "Game.h"

#include "Walnut/Input/Input.h"

#include "imgui.h"

#include<thread>

namespace Chess
{
    Game::Game()
    {
        m_Renderer = std::make_shared<Chess::Renderer>(960, 960);
        m_SpriteSheet = std::make_shared<Chess::SpriteSheet>("./res/pieces.png");
    }
    Game::~Game()
    {
    }

    void Game::OnUpdate(float ts)
    {
		if(Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Left) && pos.x !=-1 && pos.y != -1)
		{
			if(!mouseDown)
			{
				selectedSquare = (int)(pos.x*8 + pos.y);
				selectedPiece = Board::Get().GetBoard()[selectedSquare];
				mouseDown = true;
				if(selectedPiece)
					Board::Get().removePiece((int)(pos.x*8 + pos.y));
			}
			if(selectedPiece)
			{
				float x = Walnut::Input::GetMousePosition()[0];
				float y = Walnut::Input::GetMousePosition()[1]-52;
				
				m_Renderer->ClearBoard();
				drawTargetSquares(selectedSquare);
				drawBoard();
				m_Renderer->MoveSprite(Piece::PieceType(selectedPiece),Piece::Color(selectedPiece), x/scale, glm::abs(y-windowWidth)/scale);
            	if(Board::Get().numSquaresToEdge[(pos.x*8 + pos.y)][!Piece::Color(selectedPiece)]==0 && Piece::IsType(selectedPiece, Piece::Pawn))
					popup = true;
				m_Renderer->UpdateImage();
			}		
		}
		else
		{
			if(mouseDown && selectedPiece)
			{
				Board::Get().move((int)(pos.x*8 + pos.y), selectedPiece);
			}
			mouseDown = false;
			m_Renderer->ClearBoard();
			drawBoard();
			m_Renderer->UpdateImage();
		}
	}

    void Game::OnUIRender()
    {
        ImGui::Begin("Setting");
		pos = Square();
		if(Walnut::Input::IsMouseButtonDown(Walnut::MouseButton::Left) && pos.x != -1)
			ImGui::Text("square: %.f, %.f", pos.x, pos.y);
		else
			ImGui::Text("square: nan, nan");
		
		ImGui::Text("Image size %d, %d",m_Renderer->GetWidth(), m_Renderer->GetHeight());
		ImGui::Text("Window size %d, %d",windowWidth,  windowHeight);
		if(ImGui::Button("undo"))
			Board::Get().undo();
		ImGui::Text("Num of moves: %d", Board::Get().GetNumOfMoves());
		std::string turn = Board::Get().GetColorToMove()==8? "White's":"Black's";
		turn += " Turn";
		ImGui::Text(turn.c_str());
		ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Game");
		
		if(popup)
		{
			ImGui::OpenPopup("promot_to_popup");
			popup = false;
		}
		if (ImGui::BeginPopup("promot_to_popup"))
		{
			ImGui::Text("promot");
			ImGui::Separator();
			for (int i = 0; i < IM_ARRAYSIZE(promot); i++)
			{
				if(ImGui::Selectable(promot[i]))
					Board::Get().SetPromotTo(i+2);
			}
			ImGui::EndPopup();
		}

		windowWidth = ImGui::GetContentRegionAvail().x;
		windowHeight = ImGui::GetContentRegionAvail().y;

		boardSize = windowWidth < windowHeight? (float)windowWidth : (float)windowHeight;
		scale = boardSize/(m_Renderer->GetSquareWidth()*8);
        ImGui::Image(m_Renderer->GetImage()->GetDescriptorSet(), ImVec2(boardSize, boardSize), ImVec2(0, 1), ImVec2(1, 0));
        m_Renderer->OnResize(windowWidth, windowHeight);	
		ImGui::End();
		ImGui::PopStyleVar();
    }

    glm::vec2 Game::Square()
	{
		float x = Walnut::Input::GetMousePosition()[0];
		float y = Walnut::Input::GetMousePosition()[1]-52;

		if(x > 0 && y > 0 && x < windowWidth && y < windowWidth)
		{
			m_MouseX = x, m_MouseY = y;
			x = (int)(x/(m_Renderer->GetSquareWidth()*scale));
			y = (int)(y/(m_Renderer->GetSquareWidth()*scale));	
		}
		else
		{
			m_MouseX = -1;
			m_MouseY = -1;
			x = -1, y = -1;
		}
		return {glm::abs(y-7), x};
	}

	void Game::drawBoard()
	{
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < 8; j++)
			{
				int piece = Board::Get().GetBoard()[i*8 + j];
				if(piece)
					m_Renderer->DrawSprite(Piece::PieceType(piece),Piece::Color(piece), j, i);
			}
		}
	}

	void Game::drawTargetSquares(int startSquare){
		for(int x = 0; x < Board::Get().GetPossibleMoves(startSquare).size(); x++)
		{
			int targetSquare = Board::Get().GetPossibleMoves(startSquare)[x].targetSquare;
			int j = targetSquare%8;
			int i = (targetSquare - j)/8;
			m_Renderer->changeSquareColor(j, i);
		}
	}
}
    