#include "SpriteSheet.h"

#include "stb_image.h"

#include <iostream>

namespace Chess{
    SpriteSheet::SpriteSheet(const std::filesystem::path &path) : m_Path(path)
    {
        std::string filePathString = path.string();
        stbi_set_flip_vertically_on_load(true);
        m_Pixels = (uint32_t*)stbi_load(filePathString.c_str(), &m_Width, &m_Height, nullptr, 4);
        if(!m_Pixels)
            std::cout<<"Couldn't load SpriteSheet: " << filePathString << std::endl;
    }

    SpriteSheet::~SpriteSheet(){
        stbi_image_free(m_Pixels);
    }
}
