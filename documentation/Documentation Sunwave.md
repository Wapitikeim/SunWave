This documentation provides a comprehensive overview of the Sunwave codebase. It details the structure and functionality of each class, including concise descriptions, use cases, and practical examples where applicable. Whether you're looking to extend the engine, implement new features, or simply understand the architecture, this guide serves as a reference for navigating and utilizing the API effectively.

# Contents:

 [[#Summary of the codebase]]

[[#How to Implement a New Minigame?]]

[[#How to Contribute to Engine Development?]]

[[#What Features Are Missing and Could Be Easily Implemented?]]

[[#Code Quality Considerations]] 

---
# Classes Overview
---
### Entry Point
- [[#main]]

### Most Relevant Classes:
- [[#GameEnvironment]]  
  - [[#SceneManager]] - Level loading/saving  
  - [[#UiManager]] - ImGui/Sandbox  

- [[#Entity]]  
  - [[#PlayerShape]] - Implementation of player controls  
  - [[#UiElement]] - UI Elements class  

- [[#MinigameManager]] - Logic for all the minigames  

### Physics-Related Classes:
- [[#PhysicsEngine]]  
  - [[#PhysicsCollider]] - Component used by the Physics Engine  
  - [[#MortonHashTableLogic]] - Morton encoded hash table logic  
  - [[#CollisionTester]] - SAT  

### Utility classes:
- [[#MeshContainer]]  
- [[#ShaderContainer]]  
- [[#EntityFactory]]  
- [[#Component]]  
  - [[#Shape]]  
- [[#FontLoader]]  
- [[#Camera]]  
- [[#FileReader]]  
- [[#glfwPrep]]  
- [[#HelperFunctions]]  
- [[#InfiniteGrid]]  

---
# Summary of the codebase

The **Sunwave** codebase is structured around a **custom 2D physics engine**, a **scene and entity management system**, and an **interactive UI framework**. The project consists of various **minigames**, each utilizing physics-driven mechanics and a component-based design.

The **core entry point** initializes the **[[#GameEnvironment]]**, which manages the **game loop, physics updates, rendering, and input handling**. Scene management is handled by the **[[#SceneManager]]**, which supports **level loading and saving** using JSON. UI elements and debugging tools are managed by the **[[#UiManager]]**, built with **ImGui** to facilitate in-game interaction and system monitoring.

The **physics system** includes broad-phase collision detection using **Morton-Encoding [[#MortonHashTableLogic]]** and narrow-phase resolution with **Separating Axis Theorem (SAT) [[#CollisionTester]]**. Physics objects are managed via the **[[#PhysicsEngine]]**, utilizing **PhysicsColliders** for accurate collision handling and response.

The **entity-component system** allows for modular game object design, with specialized components like **[[#PlayerShape]]**, ** [[#UiElement]]**, and **[[#PhysicsCollider]]**. The **[[#MinigameManager]]** provides logic for distinct game modes and structured gameplay progression.

Supporting utilities include **[[#MeshContainer]]** and **[[#ShaderContainer]]** for rendering, **[[#FontLoader]]** for text rendering via FreeType, and **[[#FileReader]]** for asset management. The **[[#Camera]]** system ensures updates on all relevant matrices, and **[[#glfwPrep]]** initializes the OpenGL context.

The documentation provides a detailed breakdown of these components, offering insights into **engine architecture, class interactions, and best practices** for further development.

---
# How to Implement a New Minigame?

A good starting point is to examine the **[[#MinigameManager]]** class, which orchestrates the logic and structure of all minigames. By analyzing existing implementations, you can gain insights into the framework and how different game mechanics are abstracted.

To add a new minigame, register it within the **[[#GameEnvironment]]**, utilizing its built-in function registration system. This ensures that your game integrates seamlessly into the existing minigame rotation, benefiting from pre-configured update cycles, physics interactions, and UI elements.

Additionally, if your game requires unique **collision rules, input handling, or rendering behavior**, consider extending relevant classes like **[[#PhysicsEngine]]**, **[[#UiManager]]**, or **[[#Entity]]**.

---

# How to Contribute to Engine Development?

If you’re looking to expand the engine’s capabilities, the **sandbox mode** (accessible through **[[#UiManager]]**) provides a great foundation for experimentation. It allows runtime modifications, making it easier to test new mechanics, physics interactions, and UI features.

For structured improvements, refer to **[[#What features are missing and could be easily implemented?]]**, which outlines key areas where enhancements would bring significant value. Changes in core systems, such as physics or rendering, should be carefully benchmarked to avoid regressions in performance or stability.

---

# What Features Are Missing and Could Be Easily Implemented?

The current engine architecture leaves room for several useful additions, including:

- **A centralized sound manager** (currently handled ad hoc in **[[#GameEnvironment]]**)
- **A dedicated mouse input manager** for better input abstraction
- **A more advanced camera system** with zooming, smoothing, and tracking features
- **A standalone sandbox mode executable** to isolate debugging tools from gameplay

These enhancements would improve modularity and maintainability, making future development more efficient.

---

# Code Quality Considerations

While the codebase is functional and modular, several areas could be improved:

- **Physics engine stability:** Some noticeable issues exist regarding **colliders sinking into the ground**, which affects gameplay accuracy.
- **Smart pointer enforcement:** Although partially implemented, the use of **smart pointers** could be more rigorously applied to avoid potential memory leaks.
- **Namespace organization:** Currently, the structure lacks **consistent namespaces**, making it harder to manage dependencies across different subsystems.

Addressing these issues would significantly enhance code maintainability, performance, and readability.

---
# main 
## Overview

Simple entry point that:

1. Creates GameEnvironment instance
2. Runs the game loop
3. Handles any unhandled exceptions

## Purpose

- Creates single instance of game engine
- Exception handling for graceful error reporting
- Program entry and exit point

---
# Most Relevant Classes:
---
# GameEnvironment
## Core Functionality

The GameEnvironment class serves as the main game engine, handling rendering, physics, input, and game state management. Also currently mouse interaction and update.

## Key Components

### Entity Management

```C++
// Add a new entity to the game
void addEntity(std::unique_ptr<Entity> entityToAdd);

// Get entity by name and type
template<typename entityTypeToGet>
entityTypeToGet* getEntityFromName(std::string entityName);

// Remove entity by name
void deleteEntityFromName(std::string entityName);
```
### Menu State Control

```C++
// Pause/unpause the game
void setGamePaused(const bool& newPaused);

// Control menu states
void setInMenu(const bool& newMenu);
void setInLevelSelector(const bool& newLevelSelector);

// Load different game states
void loadMenu();
void loadLevelSelector();
```
### Function Registration System

```C++
// Execute function after delay
void registerFunctionToExecuteWhen(float whenFunctionShouldStart, std::function<void()> functionToExecute);

// Execute function repeatedly until condition is met
void registerRepeatingFunction(std::function<void()> functionToExecute, std::function<bool()> stopCondition);

// Execute function for specified duration
void registerLoopingFunctionUntil(std::function<void()> functionToExecute, float secondsToRun);
```
### Mouse Interaction

```C++
// Enable/disable entity manipulation through mouse
void setMouseEntityManipulation(const bool& newM);

// Enable/disable hover effects
void setHoverOverEffect(const bool& newHover);
void setHoverOverColor(const glm::vec4& newColor);

// Get mouse position
const float& getMouseX() const;
const float& getMouseY() const;
```
## Common Use Cases

### Adding Interactive UI Elements

```C++
// Create a UI button with click functionality
auto button = std::make_unique<UiElement>("ButtonName", position, scale, rotation, "Button Text", "FontPath", fontSize);

button->setOnClick([this]()
{
    // Button click logic here
});

addEntity(std::move(button));
```
### Setting Up Physics Objects

```C++

// Create a physics-enabled entity

auto entity = std::make_unique<Shape>("EntityName", position, scale, rotation);
entity->addComponent(std::make_unique<PhysicsCollider>(entity.get(), boolIfStaticOrNot);
getPhysicsEngine()->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("EntityName", "Physics"));

addEntity(std::move(entity));
```
### Scheduling Game Events

```C++

// Execute after 5 seconds
registerFunctionToExecuteWhen(5.0f, []() 
{
    // Delayed logic here
});

// Repeat until condition met
registerRepeatingFunction(
    []() { /* repeated logic */ },
    []() -> bool { return /* condition */; }
);

//Looping function for 5 seconds
registerLoopingFunctionUntil
(
	[](){/* looping logic */},
	5.f
);


```
### Scene Management
```C++

// Load a new level
getSceneManager().loadLevel("LevelName", getEntities(), getPhysicsEngine());

// Reset game state
prepareForLevelChange();
resetMouseStates();
resetRegisterdFunctions();
```

### Sound
```C++
// in loadAllMusic add files to assets/audio/..
std::vector<std::pair<std::string, std::string>> musicFiles = 
- {
	{"menu", "BackgroundMusic.wav"},
	{"shape", "Shape.wav"},
	{"go", "Go.wav"},
	{"catch", "Catch.wav"},
	{"sunwave", "Sunwave.wav"},
	// Add more music files here
};

//To play Music
playMusicByName("menu");
```
## Important Notes

- Always check for nullptr when getting entities by name
- Physics engine updates automatically in the game loop
- Function update system runs in the game's update loop

## Best Practices

1. Use RAII with std::unique_ptr for entity management
2. Implement safeguard if registering repeating functions
3. Handle game state transitions cleanly
4. Use the function registration system for timed events

## Related Components

- [[#PhysicsEngine]]
- [[#sceneManager]]
- [[#UiManager]]
- [[#MinigameManager]]
- [[#Entity]]
- [[#UiElement]]
- [[#PlayerShape]]

---
# SceneManager
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

---
# UiManager
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
- [[#GameEnvironment]]
- [[#PhysicsEngine]]
- [[#sceneManager]]

---
# Entity

## Core Purpose
Base class for all game objects that provides:
1. Transform functionality (position, rotation, scale)
2. Component system
3. Rendering capabilities
4. Unique identification (currently the name!)

## Class Structure

### Core Properties
```cpp
private:
    std::string entityName;       // Unique identifier
protected:
    std::string type;            // Entity type identifier
    glm::vec3 entityPosition;    // World position
    glm::vec3 entityScale;       // Object scale
    float entityRotation;        // Z-axis rotation
    ShaderContainer entityShader;// Rendering shader
    MeshContainer entityMesh;    // Visual mesh
```

### Component System
```cpp
// Add component
entity->addComponent(std::make_unique<CustomComponent>());

// Get component
auto* component = entity->getComponent("ComponentName");

// Remove component
entity->removeComponent("ComponentName");
```

### Transform Operations
```cpp
// Position
entity->setPosition(glm::vec3(x, y, z));
glm::vec3 pos = entity->getPosition();

// Rotation
entity->setZRotation(45.0f);
float rot = entity->getRotation();

// Scale
entity->setScale(glm::vec3(2.0f, 2.0f, 1.0f));
glm::vec3 scale = entity->getScale();
```

## Creating Custom Entities

### Basic Entity
```cpp
class MyEntity : public Entity {
public:
    MyEntity(const std::string& name, glm::vec3 pos) 
        : Entity(name, 
                ShaderContainer("basicShader"), 
                MeshContainer("basicMesh"), 
                pos,
                glm::vec3(1.0f),
                0.0f) {
        type = "MyEntity";
    }
    
    void update(GLFWwindow* window, float deltaTime) override {
        Entity::update(window, deltaTime);
        // Custom update logic
    }
};
```

### Component-Based Entity
```cpp
// Create entity with components
auto entity = std::make_unique<Entity>("Player", shader, mesh, position, scale, rotation);
entity->addComponent(std::make_unique<PhysicsComponent>());
entity->addComponent(std::make_unique<InputComponent>());
```

## Best Practices
1. Keep in mind the name is the UUID
2. Check component existence before access
3. Override update() for custom behavior

## Common Use Cases

### Dynamic Object
```cpp
auto dynamicObject = std::make_unique<Entity>("Player",
    ShaderContainer("characterShader"),
    MeshContainer("playerMesh"),
    glm::vec3(0.0f),
    glm::vec3(1.0f),
    0.0f
);
dynamicObject->addComponent(std::make_unique<PhysicsCollider>(dynamicObject.get(), false));
```

### Related Code
- [[#PlayerShape]]
- [[#UiElement]]

---
# PlayerShape

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

---
# UiElement
## Overview
Implementation of Entity class for UI/text rendering with click functionality.

## Core Features
1. Text rendering with TTF fonts  (available Fonts found under ui/Fonts/...)
2. Click event handling
3. Dynamic text sizing
4. Color customization
5. World-to-screen space conversion

## Example Usage
```cpp
// Create clickable text button
auto button = std::make_unique<UiElement>(
    "startButton",               // name
    glm::vec3(0.0f),            // position
    glm::vec3(1.0f),            // scale
    0.0f,                       // rotation
    "Click Me!",                // text
    "OpenSans.ttf",                // font
    32                          // font size
);

// Add click behavior
button->setOnClick([]() {
    // Click handler
    std::cout << "Button clicked!" << std::endl;
});

// Customize appearance
glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);  // Red
button->setTextColor(color);

// Add collision for click detection
button->addComponent(std::make_unique<PhysicsCollider>(button.get(), true));
```

Key features:
- TTF font rendering
- Automatic text size calculation
- Screen space positioning
- Click event callbacks
- Color customization

---
# MinigameManager
## Core Purpose
Manages different minigames, their transitions, difficulty progression, and scoring system.

## Key Components

### Minigame Types
```cpp
enum class MinigameType {
    FindShape,      // Find specific shape among many
    GoToPosition,   // Navigate to target location
    Catch,          // Catch falling objects
    Sunwave,        // Combined minigame sequence
    None           
};

enum class Difficulty {
    Easy,           // Basic gameplay
    Middle,         // Increased complexity
    Hard            // Maximum challenge
};
```

### Game Flow Control
```cpp
// Start specific minigame
void startMinigame(MinigameType type);

// Reset game state
void resetMinigameVariabels();

// Handle game transitions
void handleNextMinigame();
void blendTheNextGame();
```

### Minigame Implementations
```cpp
// Core minigames
void miniGameFindShape();    // Shape finding puzzle
void miniGameGoToPosition(); // Movement challenge
void miniGameCatch();        // Object catching game
void startSunwaveGame();     // Combined challenge sequence
```

## Example Usage

### Starting a Single Minigame
```cpp
auto gameManager = std::make_unique<MinigameManager>(gameEnvironment);
gameManager->startMinigame(MinigameType::FindShape);
```

### Running Sunwave Sequence
```cpp
// Starts sequence of randomized minigames
gameManager->startMinigame(MinigameType::Sunwave);
```

## Key Features
1. Progressive difficulty system
2. High score tracking
3. Game state persistence
4. Seamless transitions
5. Tutorial/explanation screens
6. Multiple game modes
7. Performance tracking
8. Dynamic object spawning

## Important Variables
```cpp
timeToComplete          // Time taken for completion
shapesHandeldCorrectly  // Current score
roundsPlayed           // Game progression tracker
currentDifficulty      // Current difficulty level
minigameSequence       // Planned game sequence
```

## Related Components
- [[#GameEnvironment]]
- [[#PhysicsEngine]]
- [[#sceneManager]]

---
# Physics-Related Classes:
---
# PhysicsEngine
## Core Overview

The PhysicsEngine manages physics simulation, collision detection, and resolution for physics colliders. It uses a morton encoded hash table for broad collision phase and SAT for narrow.

## Key Components

### Physics Object Management
```C++
// Register a collider with the physics engine
void registerPhysicsCollider(PhysicsCollider* colliderToRegister);

// Remove a collider from the physics engine
void unregisterCollider(PhysicsCollider* colliderToRemove);

// Clear all physics objects
void clearPhysicsObjects();
```
### Physics Simulation Control
```C++
// Control simulation speed and quality
float speedOfSimulation = 1;        // Default speed multiplier
float tickrateOfSimulation = 150;   // Physics updates per second

// Pause/resume physics simulation
void setIsHalting(const bool& newHalt);

// Get simulation step time
float getTimeStep() { return (1.0f / tickrateOfSimulation) * speedOfSimulation; }
```
### Collision Detection from outside the physicsengine
```C++
// Check if an object would collide at given position
bool checkIfShellWouldCollide(glm::vec3& pos, glm::vec3& scale, float& rotZ);

// Check collision between named entities
bool checkTriggerColliderCollision(const std::string& colliderEntityName, 

                                 const std::string& triggerColliderEntityName);

// Check collision with player
bool checkColliderPlayerCollision(const std::string& colliderEntityName);
```
## Constants & Parameters

### Physics Properties
```C++
float GRAVITY = -9.81f;           // Gravity force
float AIR_RESISTANCE = 0.995f;    // Air resistance multiplier
float RESTING_THRESHOLD = 0.15f;  // Minimum velocity for movement
float BOUNCE_MULTIPLIER = 2.25f;  // Collision bounce force multiplier
```
## Common Use Cases

### Setting Up a Physical Object
```C++
// Create and register a physical object
auto physicsCollider = std::make_unique<PhysicsCollider>(entity, boolIfStaticOrNot);
physicsCollider->setElasticity(0.5f);      // Set bounce factor
physicsCollider->setMass(mass);       // adjust the mass
physicsEngine->registerPhysicsCollider(physicsCollider.get());
```
### Collision Response Configuration
```C++
// Configure collision properties
physicsCollider->setIsTrigger(true);       // Trigger only, no physical response
physicsCollider->setUnaffectedByGravity(true);  // Floating object
physicsCollider->setLockX(true);           // Constrain X-axis movement
```
### Performance Monitoring
```C++
// Get physics engine statistics
int collisionsLastFrame = getTicksLastFrame();
int collisionsPerSecond = getTicksLastSecond();
int activeCollisions = getCurrentCollisions();
```
## Important Notes

1. Objects must be registered before physics simulation affects them
2. Trigger colliders don't cause physical responses but register as collisions
3. Static objects don't move but affect dynamic objects
4. Performance depends on number of active colliders
5. Higher tickrates increase accuracy but cost performance

## Best Practices

6. Use static colliders for immobile objects
7. Set appropriate mass values for realistic behavior
8. Use triggers for detection without physical response
9. Clear unused colliders to maintain performance
10. Balance tickrate with performance requirements

## Physics Steps

11. Broad phase (Morton hash table spatial partitioning)
12. Narrow phase (precise collision detection through Seperated Axis Theorem (SAT) )
13. Collision response (impulse-based resolution)
14. Position correction (prevent sinking)
15. Force integration (gravity, velocity updates)

## Related Components

- [[#PhysicsCollider]]
- [[#CollisionTester]]
- [[#MortonHashTableLogic]]
- [[#Entity]]

---
# PhysicsCollider
## Core Overview
PhysicsCollider is a component that adds physical properties and collision detection capabilities to entities.

## Key Structures

### PhysicsBody
```cpp
struct PhysicsBody {
    glm::vec3 colliderPosition;    // World position
    glm::vec3 colliderScale;       // Size/scale
    float colliderZRotation;       // Rotation around Z axis
    glm::vec3 colliderVelocity;    // Current velocity
    glm::vec3 colliderAcceleration;// Current acceleration
    glm::vec3 accumulatedForce;    // Sum of forces
    float mass;                    // Object mass
};
```

### CornerPositions
```cpp
struct CornerPositions {
    glm::vec3 leftBottom;
    glm::vec3 leftTop;
    glm::vec3 rightTop;
    glm::vec3 rightBottom;
    std::vector<glm::vec3> cornerVec;
};
```

## Usage

### Creating a PhysicsCollider
```cpp
// Create dynamic physics collider
auto entity = std::make_unique<Entity>("DynamicObject");
auto collider = std::make_unique<PhysicsCollider>(entity.get(), false);

// Create static physics collider
auto staticEntity = std::make_unique<Entity>("StaticObject");
auto staticCollider = std::make_unique<PhysicsCollider>(staticEntity.get(), true);
```

### Physics Properties
```cpp
// Configure physics behavior
collider->setMass(1.0f);
collider->setElascity(0.5f);           // Bounce factor (0-1)
collider->setUnaffectedByGravity(true);
collider->setIsTrigger(true);          // Collision detection only
```

### Force Application
```cpp
// Apply forces
collider->applyForce(glm::vec3(0.0f, 10.0f, 0.0f));  // Apply upward force
collider->clearForces();                              // Reset forces
```

### Movement Constraints
```cpp
collider->setLockX(true);   // Lock X-axis movement
collider->setLockY(true);   // Lock Y-axis movement
```

### Collision State
```cpp
bool isGrounded = collider->getIsGrounded();
bool isInContact = collider->getIsInContact();
bool isResting = collider->getIsResting();
```

## Properties

### Physical Properties
- Mass (default: 1.0)
- Elasticity (default: 0.2)
- Static/Dynamic state
- Trigger mode
- Grounded state
- Contact state
- Resting state

### Transform Data
- Position (World space)
- Scale (Size)
- Rotation (Z-axis only)
- Corner positions (For collision)

## Important Methods
```cpp
void update();                    // Update physics state
void updateCornerPositions();     // Recalculate collision bounds
void applyForce(glm::vec3);      // Add force to object
void clearForces();              // Reset accumulated forces
```

## Best Practices
1. Set appropriate mass for realistic physics
2. Use triggers for detection without physics
3. Mark static objects appropriately
4. Update transforms through physics engine
5. Clear forces when resetting object state

## Common Configurations

### Platform
```cpp
auto platform = std::make_unique<PhysicsCollider>(entity, true);
platform->setElascity(0.5f);
```

### Dynamic Object
```cpp
auto dynamic = std::make_unique<PhysicsCollider>(entity, false);
dynamic->setMass(1.0f);
dynamic->setElascity(0.2f);
```

### Trigger Zone
```cpp
auto trigger = std::make_unique<PhysicsCollider>(entity, true);
trigger->setIsTrigger(true);
trigger->setUnaffectedByGravity(true);
```

---
# MortonHashTableLogic
## Overview
Spatial partitioning system using Morton encoding (Z-order curve) for efficient broad-phase collision detection.

## Core Purpose
1. Converts 2D space into grid cells
2. Maps objects to grid using Morton codes
3. Enables quick spatial queries
4. Reduces collision check pairs

## Technical Implementation
```cpp
class MortonHashTableLogic {
private:
    const int32_t OFFSET = 50;        // Shift for negative coordinates
    const float BUCKETSIZE = 2.0f;    // Grid cell size
    const float CORRECTION_DIVIDER = 2.0f;  // Subdivision factor
};
```

## Key Operations

### Morton Encoding
```cpp
// Convert 2D coordinates to Morton code
uint32_t mortonCode = mortonEncode2D(x, y);

// Add collider to spatial hash
void addColliderIntoHashTable(PhysicsCollider* collider, 
                            std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& table);

// Remove collider from spatial hash
void removeColliderFromHashTable(PhysicsCollider* collider,
                               std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& table);
```

## Integration with Physics Engine
```cpp
// In PhysicsEngine class
private:
    std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> mortonHashTable;
    MortonHashTableLogic tableLogic;

// Usage in broad phase
void broadCollisionGathering() {
    // Objects in same grid cell are potential collision pairs
    for (auto& [key, colliders] : mortonHashTable) {
        if (colliders.size() > 1) {
            // Process potential collisions
        }
    }
}
```

## Performance Notes
1. Grid size (BUCKETSIZE) affects collision detection precision
2. OFFSET handles negative coordinate space
3. CORRECTION_DIVIDER prevents edge case misses
4. Efficient for uniformly distributed objects
5. O(n) space complexity for n objects

## Related 
- [[#CollisionTester]]

---
# CollisionTester
## Core Purpose
Static utility class for collision detection between 2D boxes using Separating Axis Theorem (SAT).

## Key Methods

### Primary Collision Tests
```cpp
// Basic collision check between two colliders
static bool arePhysicsCollidersColliding(PhysicsCollider* e1, PhysicsCollider* e2);

// Detailed collision info including contact normal and penetration
static bool arePhysicsCollidersCollidingWithDetails(
    PhysicsCollider* e1, 
    PhysicsCollider* e2, 
    glm::vec3& contactNormal, 
    float& penetrationDepth
);

// Test collision with a hypothetical collider "shell"
static bool isColliderCollidingWithShell(
    PhysicsCollider* e1, 
    glm::vec3& posE2, 
    glm::vec3& scaleE2, 
    float& rotzE2
);
```

### Distance Calculations
```cpp
// Signed distance between point and box
static float signedDistancePointAnd2DBox(
    glm::vec3 pointPos, 
    PhysicsCollider* colliderToCheck
);

// Signed distance between two boxes
static float signedDistance2DBoxAnd2DBox(
    PhysicsCollider* colliderOne, 
    PhysicsCollider* colliderTwo
);
```

## Implementation Details

### SAT Algorithm Steps
1. Calculate corner points for both colliders
2. Generate projection axes from edges
3. Project vertices onto each axis
4. Test for overlap on all axes
5. Calculate minimum penetration depth
6. Determine contact normal

## Key Features
1. Handles rotated boxes
2. Provides detailed collision information
3. 2D-focused implementation
4. Static utility design

## Related 
- [[#MortonHashTableLogic]]
---

# Utility classes

---

# MeshContainer
## Core Purpose
Handles OpenGL mesh data, buffers, and texture management for both 2D shapes and text rendering.

## Key Components

### Buffer Management
```cpp
private:
    unsigned int VAO;    // Vertex Array Object
    unsigned int VBO;    // Vertex Buffer Object
    unsigned int EBO;    // Element Buffer Object
    unsigned int texture;// Texture handle
```

### Creating Meshes
```cpp
// Standard mesh with texture
auto mesh = MeshContainer(
    vertexData,          // Vertex positions & UVs
    indices,             // Triangle indices
    "texture.png"        // Texture path
);

// Text rendering mesh
auto textMesh = MeshContainer(true);  // Special setup for text
```

### Drawing Operations
```cpp
// Regular mesh drawing
mesh.drawMesh();        // Textured triangles
mesh.drawLine();        // Line rendering

// Text rendering
mesh.drawText(
    "Hello World",      // Text to render
    fontLoader,         // Font data
    100.0f,            // X position
    100.0f,            // Y position
    1.0f,              // X scale
    1.0f               // Y scale
);
```

## Key Features
1. OpenGL buffer management
2. Texture loading (PNG/JPG)
3. Vertex attribute setup
4. Text rendering support

Used by:
- [[#Entity]]
- [[#UiElement]]

---
# ShaderContainer
## Core Purpose
OpenGL shader program management including compilation, linking, and uniform handling.

## Key Components

### Shader Setup
```cpp
// Create shader program with vertex and fragment shaders
auto shader = ShaderContainer(
    "vertex.glsl",     // Vertex shader path
    "fragment.glsl"    // Fragment shader path
);
```

### Uniform Management
```cpp
// Set common uniforms
shader.useShader();
shader.setUniformVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
shader.setUniformMat4("projection", projectionMatrix);
shader.setUniformFloat("time", glfwGetTime());
```

## Key Features
1. RAII shader management
2. Move semantics (no copying)
3. Automatic error checking
4. Uniform value setting

## Error Handling
```cpp
struct glStatusData {
    int success;
    const char* shaderName;
    char infoLog[GL_INFO_LOG_LENGTH];
};
```

Used by:
- [[#Entity]]
- [[#UiElement]]
- [[#MeshContainer]]

Note: Requires valid OpenGL context and shader files in correct path

---
# EntityFactory

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
5. Used by [[#GameEnvironment]] , [[#MinigameManager]] and [[#sceneManager]]

---
# Component
## Core Purpose
Base class for the entity component system. Used for extending entity functionality through composition.

## Example Usage
```cpp
// Custom component
class PhysicsComponent : public Component {
    public:
        PhysicsComponent() { componentName = "Physics"; }
        void update() override { /* Physics logic */ }
};

// Add to entity
entity->addComponent(std::make_unique<PhysicsComponent>());

// Get component
auto* physics = entity->getComponent("Physics");
```

## Key Features
6. Virtual interface for components
7. Name-based component identification 
8. Used by Entity system
9. Update mechanism for game loop integration

Used by:
- [[#PhysicsCollider]]
- [[#Entity]]

---
# Shape
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
10. Default quad mesh (2 triangles)
11. UV coordinates for texturing
12. Support for custom shaders
13. Base class for other entities
14. Integrated with EntityFactory

Used by:
- [[#MinigameManager]]
- [[#GameEnvironment]]
- [[#sceneManager]]

---
# FontLoader
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
15. TrueType font support
16. ASCII character set (0-127)
17. Texture atlas generation

Used by:
- [[#UiElement]]
- [[#MeshContainer]]

---
# Camera
## Core Purpose
Static utility class providing global access to view, projection and orthographic matrices.

## Implementation
```cpp
// Get matrices
glm::mat4 view = Camera::getCurrentCameraView();
glm::mat4 proj = Camera::getCurrentCameraProjection();
glm::mat4 ortho = Camera::getCurrentCameraOrto();

// Set matrices
Camera::setCurrentCameraView(viewMatrix);
Camera::setCurrentCameraProjection(projMatrix);
Camera::setCurrentCameraOrto(glm::ortho(0.0f, width, 0.0f, height));
```

## Key Features
18. Static access to camera matrices
19. Default orthographic setup (1920x1080)
20. Global state management

Used by:
- [[#ShaderContainer]]
- [[#GameEnvironment]]
- [[#UiElement]]

Note: Marked as temporary solution, pending scene system implementation

---
# FileReader
## Core Purpose
Static utility class for file operations, primarily handling:
21. Path management
22. Shader loading
23. Level file discovery
24. File content extraction

## Key Operations

### Path Management
```cpp
// Trim path to working directory
std::filesystem::path path = std::filesystem::current_path();
fileReader::trimDownPathToWorkingDirectory(path);
```

### Shader Loading
```cpp
// Load shader from file
std::string shaderSource = fileReader::readShader("vertex.glsl");
```

### Level Management
```cpp
// Get all available level files
std::vector<std::string> levels = fileReader::getAllLevelFileNames();

// Get specific file path
auto path = fileReader::getPathToFileInFolder(
    "level1.json",
    "LevelConfigurations"
);
```

## Key Features
25. Path normalization
26. File content reading
27. Extension filtering
28. Directory traversal
29. Error checking

Used by:
- [[#ShaderContainer]]
- [[#sceneManager]]
- [[#FontLoader]]

Note: All paths are relative to working directory (`/out/`) 

---
# glfwPrep
## Key Operations

### Window Creation
```cpp
// Create OpenGL 3.3 window with icon
GLFWwindow* window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(
    1920,           // width
    1080,           // height
    "Window Title"  // window name
);
```

### Window Size Tracking
```cpp
// Get current window dimensions
int width = glfwPrep::getCurrentWindowWidth();
int height = glfwPrep::getCurrentWindowHeight();
```

## Key Features
30. OpenGL 3.3 core profile setup
31. GLAD initialization
32. Window icon loading
33. Viewport management
34. Framebuffer resize handling

Used by:
- [[#GameEnvironment]]
- [[#UiElement]]
- [[#Camera]]

---
# HelperFunctions
## Core Purpose
Header-only utility functions for random generation needs.

## Available Functions

### Random String Generation
```cpp
// Generate random alphanumeric string
std::string id = random_string(10);  // "aB3kX9pL2q"
```

### Random Integer Generation
```cpp
// Generate random int in range
int value = getRandomNumber(1, 100);  // 1-100 inclusive
```

## Key Features
35. Header-only implementation
36. Modern C++ random generators
37. Alphanumeric character set
38. Inclusive range for integers
39. Thread-safe random device

Used by:
- [[#MinigameManager]]

Note: Uses `<random>` for better randomization than rand()

---
# InfiniteGrid
## Core Purpose
Renders an infinite grid using a shader-based approach with single quad and fragment shader math.

## Implementation Steps
40. Initialize quad mesh
41. Setup OpenGL buffers 
42. Configure blending
43. Use special grid shader
44. Draw with scale factor

## Example Usage
```cpp
// Create infinite grid
auto gridShader = ShaderContainer("grid.vert", "grid.frag");
auto grid = InfiniteGrid(std::move(gridShader));

// Draw with camera matrices
grid.drawGrid(1.0f);  // 1.0 = grid cell size

// Draw with custom view/projection
grid.drawGrid(
    viewMatrix,
    projectionMatrix,
    2.0f  // larger grid cells
);
```

## Key Features
45. Single quad rendering
46. Fragment shader grid calculation
47. Adjustable scale
48. Alpha blending
49. Camera integration

Used by:
- [[#GameEnvironment]]
- [[#UiManager]]

Note: Requires specific grid shader for line pattern generation