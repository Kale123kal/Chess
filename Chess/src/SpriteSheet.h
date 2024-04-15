#pragma once
#include <filesystem>

namespace Chess
{
    class SpriteSheet
    {
    public:
        SpriteSheet(const std::filesystem::path &path);
        ~SpriteSheet();

        uint32_t *GetPixels() const { return m_Pixels; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }

    private:
        std::filesystem::path m_Path;
        uint32_t *m_Pixels = nullptr;
        int m_Width = 0, m_Height = 0;
    };
}