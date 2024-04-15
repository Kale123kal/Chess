#include "Renderer.h"

#include "Game.h"

#include "Walnut/Random.h"

#include <iostream>

namespace Chess
{
    Renderer::Renderer(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        m_Image = std::make_shared<Walnut::Image>(m_SquareWidth*8, m_SquareWidth*8, Walnut::ImageFormat::RGBA);
        m_ImageBuffer = new uint32_t[m_SquareWidth*8*m_SquareWidth*8];
    }

    Renderer::~Renderer()
    {
        delete[] m_ImageBuffer;
    }

    void Renderer::DrawSprite(uint32_t xIndex, uint32_t yIndex, int i, int j)
    {
        auto spriteSheet = Game::Get()->GetSpriteSheet();

        uint32_t* spriteSheetPixels = spriteSheet->GetPixels();
        uint32_t spriteSheetWidth = spriteSheet->GetWidth();
        uint32_t spriteSheetHeight = spriteSheet->GetHeight();

        int spriteSize = 170;
		int xp = xIndex * spriteSize, yp = yIndex * spriteSize;

		// int middleX = m_SquareWidth/2, middleY = m_SquareWidth/2;
		int xt = m_SquareWidth*i, yt = m_SquareWidth*j;

		for(int x = 0; x < spriteSize; x++)
		{
			for(int y = 0; y < spriteSize; y++)
			{
				uint32_t color = spriteSheetPixels[(xp + x) + (yp +y) * spriteSheetWidth];
		
				if(color == 0x000000)
					continue;
				m_ImageBuffer[(xt + x) + (yt + y) * m_SquareWidth*8] = color;
			}
		}
        
    }

    void Renderer::MoveSprite(uint32_t xIndex, uint32_t yIndex, int i, int j)
    {
         auto spriteSheet = Game::Get()->GetSpriteSheet();

        uint32_t* spriteSheetPixels = spriteSheet->GetPixels();
        uint32_t spriteSheetWidth = spriteSheet->GetWidth();
        uint32_t spriteSheetHeight = spriteSheet->GetHeight();

        int spriteSize = 170;
		int xp = xIndex * spriteSize, yp = yIndex * spriteSize;

		// int middleX = m_SquareWidth/2, middleY = m_SquareWidth/2;
		int xt = i-m_SquareWidth/2, yt = j - m_SquareWidth/2;

		for(int x = 0; x < spriteSize; x++)
		{
			for(int y = 0; y < spriteSize; y++)
			{
				uint32_t color = spriteSheetPixels[(xp + x) + (yp +y) * spriteSheetWidth];
				// std::cout<<(xt + x) + (yt + y) * m_Image->GetWidth()<<std::endl;
				if(color == 0x000000)
					continue;
                int index = (xt + x) + (yt + y) * m_SquareWidth*8;
                if(index >= m_SquareWidth*8*m_SquareWidth*8)
                    break;
				m_ImageBuffer[index] = color;
			}
		}
    }

    void Renderer::ClearBoard()
    {
		for(int i = 0; i< m_SquareWidth*8; i++)
		{
			for(int j = 0; j < m_SquareWidth*8; j++)
			{
				if(i/m_SquareWidth%2)
					m_ImageBuffer[i*m_SquareWidth*8+j] = ((j/m_SquareWidth)%2? darkSquare : lightSquare);
				else
					m_ImageBuffer[i*m_SquareWidth*8+j] = (j/m_SquareWidth)%2? lightSquare : darkSquare;
			}
		}
    }

    void Renderer::OnResize(uint32_t width, uint32_t height)
    {
        if(m_Width == width && m_Height == height)
            return;
        
        m_Width = width, m_Height = height;
        
        ClearBoard();

    }

    void Renderer::UpdateImage()
    {
        m_Image->SetData(m_ImageBuffer);
    }
    
    void Renderer::changeSquareColor(int i, int j)
    {
        int xt = m_SquareWidth*i, yt = m_SquareWidth*j;

		for(int x = 0; x < m_SquareWidth; x++)
		{
			for(int y = 0; y < m_SquareWidth; y++){
				m_ImageBuffer[(xt + x) + (yt + y) * m_SquareWidth*8] &= targetSquareColor;
            }
		}
    }
} 
