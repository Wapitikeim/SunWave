# EntityFactory Documentation

## Core Purpose
Factory pattern implementation for creating game entities and UI elements.

## Usage Examples

### Game Entities
```cpp
// Create basic shape
auto shape = createEntity("Shape", "box1", 
                         glm::vec3(0), glm::vec3(1), 
                         0.0f, "basicShader");

// Create player
auto player = createEntity("PlayerShape", "player1",
                         glm::vec3(0), glm::vec3(1),
                         0.0f, "playerShader");
```

### UI Elements
```cpp
// Create text button
glm::vec4 textColor(1.0f);
auto button = createUiEntity("UiElement", "button1",
                           glm::vec3(0), glm::vec3(1),
                           0.0f, "Click Me", "Arial.ttf",
                           32, textColor);
```

## Key Features
1. Centralized entity creation
2. Type safety through string identifiers
3. Exception handling for unknown types
4. Default shader/font support
5. Used by [[GameEnvironment]] , [[MinigameManager]] and [[sceneManager]]