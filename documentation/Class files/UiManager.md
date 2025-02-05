## Core Purpose
Handles all ImGui-based debug interfaces and UI controls for the game engine.
This is basically a mini editor/engine like to allow building levels easily while monitoring data.

## Key Components

### Control Panel Windows
```cpp
// Main control windows
void drawImGuiControlPanel();       // Main UI toggle panel
void drawImGuiWorldControl();       // Entity manipulation
void drawImGuiInfoPanel();         // Performance stats
void drawImGuiLevelManager();      // Scene loading/saving
void drawImGuiPhysicsEngineControl(); // Physics settings
void drawImGuiPlayerExtraInfo();   // Player debug info
void drawImGuiMouseInformation();  // Mouse interaction data
void drawImGuiEntityAdder();       // Entity creation tool
```

### Window Management
```cpp
// Toggle specific UI windows
void setShowImGuiUI(const bool& show);
void setShowGrid(const bool& show);
void setGridSize(const float& size);

// Frame handling
void prepFrames();    // Prepare new ImGui frame
void draw();          // Render all active windows
```

## Usage Examples

### Basic Setup
```cpp
// Create UI manager
auto uiManager = std::make_unique<UiManager>(window, gameEnvironment);

// Main loop
while (!glfwWindowShouldClose(window)) {
    uiManager->prepFrames();
    // ... game logic ...
    uiManager->draw();
}
```

### Entity Manipulation Example
```cpp
// World Control window features
- Entity selection via combo box
- Transform manipulation (position, scale, rotation)
- Physics properties editing
- Component inspection
- Entity deletion
```

### Scene Management Example
```cpp
// Level Manager window features
- Load/save levels
- Scene configuration
- Physics settings for loading
```

## Key Features
1. Real-time entity inspection/editing
2. Physics engine parameter tuning
3. Debug information display
4. Scene management tools
5. Entity creation wizard
6. Performance monitoring
7. Grid visualization controls
8. Player debug information

## Best Practices
1. Keep UI enabled during development
2. Use info panel for performance monitoring
3. Utilize entity adder for quick prototyping
4. Save levels frequently
5. Monitor physics engine state

## Related Components
- [[GameEnvironment]]
- [[PhysicsEngine]]
- [[sceneManager]]