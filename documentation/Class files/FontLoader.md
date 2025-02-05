## Core Purpose
Loads and manages TrueType fonts using FreeType2, converting glyphs to OpenGL textures for text rendering.

## Key Components

### Character Structure
```cpp
struct Character {
    unsigned int TextureID;  // OpenGL texture
    glm::ivec2 Size;        // Glyph dimensions
    glm::ivec2 Bearing;     // Baseline offset
    unsigned int Advance;    // Horizontal advance
};
```

### Font Loading
```cpp
// Load font with specific size
auto font = FontLoader("Arial.ttf", 32);  // 32px size

// Access character data
const auto& chars = font.getCharacters();
Character ch = chars.at('A');
```

## Key Features
1. TrueType font support
2. ASCII character set (0-127)
3. Texture atlas generation

Used by:
- [[UiElement]]
- [[MeshContainer]]
