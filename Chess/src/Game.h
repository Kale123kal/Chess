#pragma once 

#include "SpriteSheet.h"
#include "Renderer.h"
#include "Board.h"


#include "Walnut/Layer.h"
#include "glm/glm.hpp"

#include <memory>

namespace Chess
{
    class Game : public Walnut::Layer
    {
    public:
        Game();
        virtual ~Game();

        virtual void OnUpdate(float ts) override;
        virtual void OnUIRender() override;

        static std::shared_ptr<Game> Get();

        std::shared_ptr<Chess::SpriteSheet> GetSpriteSheet() const { return m_SpriteSheet; }
    private:
        glm::vec2 Square();
        void drawBoard();
        void drawTargetSquares(int startSquare);
        glm::vec4 color;

        std::shared_ptr<Chess::SpriteSheet> m_SpriteSheet;
        std::shared_ptr<Chess::Renderer> m_Renderer;
        uint32_t windowWidth, windowHeight;
        uint32_t m_MouseX, m_MouseY;
        float boardSize;
        glm::vec2 pos;
        float scale;
        int promotTo = 1;
        bool mouseDown = false;
		int selectedPiece;
        const char* promot[4] = {"Queen", "Bishop", "knight", "Rook"}; 
        bool popup = false;
        int selectedSquare;
    };
}