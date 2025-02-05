## Core Purpose
Basic renderable entity that provides a 2D quad with texture/shader support.

## Implementation Examples

### Creating Shapes
```cpp
// With texture
auto texturedShape = std::make_unique<Shape>(
    "block1",             // name
    glm::vec3(0.0f),     // position
    glm::vec3(1.0f),     // scale
    0.0f,                // rotation
    "block.png"          // texture path
);

// With custom shader
auto shadedShape = std::make_unique<Shape>(
    "block2", 
    glm::vec3(0.0f),
    glm::vec3(1.0f),
    0.0f,
    true,               // use shader
    "customShader"      // shader name
);
```

## Key Features
1. Default quad mesh (2 triangles)
2. UV coordinates for texturing
3. Support for custom shaders
4. Base class for other entities
5. Integrated with EntityFactory

Used by:
- [[MinigameManager]]
- [[GameEnvironment]]
- [[sceneManager]]