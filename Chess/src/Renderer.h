#pragma once

#include "Walnut/Image.h"

#include <memory>

namespace Chess
{
    class Renderer
    {
    public:
        Renderer(uint32_t width, uint32_t height);
        ~Renderer();
        void DrawSprite(uint32_t xIndex, uint32_t yIndex, int i, int j);
        void MoveSprite(uint32_t xIndex, uint32_t yIndex, int i, int j);

        void Clear(uint32_t clearColor = 0xff000000);
        void ClearBoard();

        void OnResize(uint32_t width, uint32_t height);

        uint32_t GetWidth() const { return m_Width; }
        uint32_t GetHeight() const { return m_Height; }

        uint32_t* GetLightSquareV(){ return &lightSquare; }
        uint32_t* GetDarkSquareV(){ return &darkSquare; }



        uint32_t GetSquareWidth() const { return m_SquareWidth; }
        uint32_t* GetImageBuffer()const {return m_ImageBuffer;}
        std::shared_ptr<Walnut::Image> GetImage() const { return m_Image; }

        void UpdateImage();
        void changeSquareColor(int i, int j);
        uint32_t targetSquareColor = 0x33ff0000;
    private:
        std::shared_ptr<Walnut::Image> m_Image;
        uint32_t* m_ImageBuffer = nullptr;
        uint32_t m_Width = 0, m_Height = 0; 
        uint32_t m_SquareWidth = 170;
        uint32_t darkSquare = 0xff536491, lightSquare = 0xffb9d2eb;
    };
}