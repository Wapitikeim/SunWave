## Core Purpose
Handles scene serialization and deserialization using JSON for:
- Entity data
- Physics properties
- UI elements
- Camera settings

## Key Operations

### Loading Scenes
```cpp
// Load a complete scene from JSON
void loadLevel(
    const std::string& levelName,
    std::vector<std::unique_ptr<Entity>>& entities,
    PhysicsEngine* physicsEngine
);

// Configure physics loading behavior
void setApplyPhysicsWhenLoading(const bool& newApply);
```

### Saving Scenes
```cpp
// Save current scene state to JSON
void saveLevel(
    const std::string& levelName,
    std::vector<std::unique_ptr<Entity>>& entities,
    PhysicsEngine* physicsEngine,
    const glm::vec3& cameraPos,
    const float& fov
);
```

## JSON Structure Example
```json
{
    "levelName": "level1",
    "entities": [
        {
            "name": "player",
            "position": [0, 0, 0],
            "scale": [1, 1, 1],
            "rotation": 0,
            "Type": "PlayerShape",
            "Physics": {
                "IsStatic": false,
                "Mass": 1.0,
                "Velocity": [0, 0, 0]
                // ...more physics properties
            }
        }
    ],
    "worldData": [
        {
            "CameraPos": [0, 0, 10],
            "Fov": 45.0
        }
    ],
    "physicsEngine": [
        {
            "BounceM": 2.25
        }
    ]
}
```

## Key Features
1. JSON-based serialization
2. Complete scene state persistence
3. Physics state management
4. Entity hierarchy support
5. UI element serialization

## Note:
Saved level configurations managed by the scene manager are located in **"scenes/LevelConfigurations"**. This is also the default path that `.loadLevel` searches for.

