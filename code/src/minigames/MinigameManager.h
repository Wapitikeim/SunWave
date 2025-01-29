#pragma once
#include <map>
#include <string>
#include <functional>
class GameEnvironment;
class Entity;

enum class MinigameType 
{
    FindShape,
    GoToPosition,
    Catch,
    Sunwave,
    None
};

enum class Difficulty 
{
    Easy,
    Middle,
    Hard
};

class MinigameManager 
{
private:
    GameEnvironment* gameEnv;
    MinigameType currentMinigame = MinigameType::None;
    Difficulty currentDifficulty = Difficulty::Easy;
    int roundsPlayed = 0;
    float timeElapsed = 0.0f;
    bool shapeFound = false;
    int entitiesToFill = 60;
    float timeToComplete = 0.f;
    int shapesSpawned = 0;
    int shapesHandeldCorrectly = 0;
    int bias = 2;
    float spawnInterval = 0.05f;
    float positionAlternation = 0.1f;
    std::map<int,std::vector<Entity*>> activeShapesMap;
    
    // Minigame transition logic
    MinigameType determineNextMinigame();
    void initializeMinigame(MinigameType type);
    void miniGameFindShape();
    void miniGameGoToPosition();
    void miniGameCatch();

public:
    MinigameManager(GameEnvironment* env) : gameEnv(env) {}
    void resetMinigameVariabels();
    void startMinigame(MinigameType type);
    void endCurrentMinigame();
    void update(float deltaTime);
    
    // Getters/Setters
    MinigameType getCurrentMinigame() const { return currentMinigame; }
    Difficulty getCurrentDifficulty() const { return currentDifficulty; }
    
    //std::string& difficultyToString(Difficulty &difficulty){return "";};

};