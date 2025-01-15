#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <map>
#include <string>
#include <glm/glm.hpp>

struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2 Size;        // Size of glyph
    glm::ivec2 Bearing;     // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Offset to advance to next glyph
};

class FontLoader {
public:
    FontLoader(const std::string& fontPath, unsigned int fontSize);
    ~FontLoader();
    const std::map<char, Character>& getCharacters() const { return Characters; }

private:
    std::map<char, Character> Characters;
    FT_Library ft;
    FT_Face face;
};