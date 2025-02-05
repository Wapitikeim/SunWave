# PlayerShape Documentation

## Overview
Example implementation of the Entity base class demonstrating:
1. Physics-based movement
2. Input handling

## Example Usage
```cpp
// Create player with texture
auto player = std::make_unique<PlayerShape>(
    "player1",                    // name
    glm::vec3(0.0f),             // position
    glm::vec3(1.0f),             // scale
    0.0f,                        // rotation
    "playerTexture.png"          // texture path
);

// Create player with custom shader
auto customPlayer = std::make_unique<PlayerShape>(
    "player2",
    glm::vec3(0.0f),
    glm::vec3(1.0f),
    0.0f,
    true,                        // use fragment shader
    "customPlayerShader"         // shader name
);

// Add physics
player->addComponent(std::make_unique<PhysicsCollider>(player.get(), false));
```

Key features:
- WASD movement with physics
- Customizable velocity (default 10.0f)
- Square mesh with texture coordinates
- Support for custom shaders/textures