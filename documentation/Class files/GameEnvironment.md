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

- [[PhysicsEngine]]
- [[sceneManager]]
- [[UiManager]]
- [[MinigameManager]]
- [[Entity]]
- [[UiElement]]
- [[PlayerShape]]