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
- [[GameEnvironment]]
- [[PhysicsEngine]]
- [[sceneManager]]