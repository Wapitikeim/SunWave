#include "MinigameManager.h"
#include "../GameEnvironment.h"
#include "../util/HelperFunctions.h"
#include <algorithm>

void MinigameManager::handleNextMinigame()
{
    //This will called when a minigame is finished
    roundsPlayed++;
    //Difficulty increase based on rounds played
    if(roundsPlayed > 2)
        currentDifficulty = Difficulty::Middle;
    if(roundsPlayed > 5)
        currentDifficulty = Difficulty::Hard;
    if(roundsPlayed > 8)
    {
        gameEnv->resetMouseStates();
        gameEnv->setMouseEntityManipulation(false);
        this->resetMinigameVariabels();
        gameEnv->loadMenu();
    }
    
    if(!minigameSequence.empty()) //Sunwave
    {
        if(currentMinigame == MinigameType::FindShape)
        {
            gameEnv->getPhysicsEngine()->setIsHalting(false);
            gameEnv->deleteEntityFromName("WallBottom");
            if(minigameSequence[roundsPlayed] == MinigameType::Catch)
            {
                this->shapesSpawned = 0;
                this->shapesHandeldCorrectly = 0;
            }
            gameEnv->registerFunctionToExecuteWhen(4.f,[this]() {this->startMinigame(minigameSequence[roundsPlayed]);});
            return;
        }
        currentMinigame = minigameSequence[roundsPlayed];
    } 
        
    
    if(currentMinigame == MinigameType::FindShape)
    {
        if(minigameSequence.empty())
        {
            gameEnv->getPhysicsEngine()->setIsHalting(false);
            gameEnv->deleteEntityFromName("WallBottom");
            gameEnv->registerFunctionToExecuteWhen(4.f,[this]() {this->miniGameFindShape();});
        }
        else
        {
            this->miniGameFindShape();
        }         
    }
    if(currentMinigame == MinigameType::GoToPosition)
    {
        this->miniGameGoToPosition();
    } 
    if(currentMinigame == MinigameType::Catch)
    {
        this->shapesSpawned = 0;
        this->shapesHandeldCorrectly = 0;
        this->miniGameCatch();
    }
}

void MinigameManager::miniGameFindShape()
{
    gameEnv->prepareForLevelChange();

    gameEnv->getPhysicsEngine()->setIsHalting(true);
    shapeFound = false;

    //Adjustments based on Difficulty
    switch (currentDifficulty)
    {
        case Difficulty::Easy:
            entitiesToFill = getRandomNumber(80, 120);
            break;
        case Difficulty::Middle:
            entitiesToFill = getRandomNumber(120, 160);
            break;
        case Difficulty::Hard:
            entitiesToFill = getRandomNumber(180, 250);
            break;
    };

    //Timer
    gameEnv->addEntity(std::make_unique<UiElement>("Timer",glm::vec3(3.3,24,0),glm::vec3(1),0,"Time: 0","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
    gameEnv->getEntityFromName<Entity>("Timer")->addComponent(std::make_unique<PhysicsCollider>(gameEnv->getEntityFromName<Entity>("Timer"),1));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Timer","Physics"));
    //Walls
    {
        gameEnv->addEntity(std::make_unique<Shape>("WallBottom", glm::vec3(gameEnv->getXHalf(),-0.7f,0.3f),glm::vec3(gameEnv->getXHalf(),1.f,1.0f), 0.0f, true, "box"));
        auto WallBottom = gameEnv->getEntityFromName<Entity>("WallBottom");
        WallBottom->addComponent(std::make_unique<PhysicsCollider>(WallBottom,1));
        gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("WallBottom","Physics"));

        gameEnv->addEntity(std::make_unique<Shape>("WallTop", glm::vec3(gameEnv->getXHalf(),gameEnv->getXHalf()*2+0.7,0.3f),glm::vec3(gameEnv->getXHalf(),1.f,1.0f), 0.0f, true, "box"));
        auto WallTop = gameEnv->getEntityFromName<Entity>("WallTop");
        WallTop->addComponent(std::make_unique<PhysicsCollider>(WallTop,1));
        gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("WallTop","Physics"));

        gameEnv->addEntity(std::make_unique<Shape>("wallLeft", glm::vec3(-0.7f,gameEnv->getYHalf(),0.3f),glm::vec3(1.0f,gameEnv->getYHalf(),1.0f), 0.0f, true, "box"));
        auto wallLeft = gameEnv->getEntityFromName<Entity>("wallLeft");
        wallLeft->addComponent(std::make_unique<PhysicsCollider>(wallLeft,1));
        gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("wallLeft","Physics"));
    
        gameEnv->addEntity(std::make_unique<Shape>("wallRight", glm::vec3(gameEnv->getXHalf()*2+0.7f,gameEnv->getYHalf(),0.3f),glm::vec3(1.f,gameEnv->getYHalf(),1.0f), 0.0f, true, "box"));
        auto wallRight = gameEnv->getEntityFromName<Entity>("wallRight");
        wallRight->addComponent(std::make_unique<PhysicsCollider>(wallRight,1));
        gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("wallRight","Physics"));
    }
    //Random Entities
    {
        //Easy pairs
        std::vector<std::string>shapeNamesEasy1{"box", "circle", "triangle"};
        std::vector<std::string>shapeNamesEasy2{"boxWithin", "circle", "star", "triangle"};
        std::vector<std::string>shapeNamesEasy3{"box", "circle", "star", "triangle", "cross"};
        //Middle pairs
        std::vector<std::string>shapeNamesMiddle1{"box", "boxWithin", "triangle", "triangleWithin", "star"};
        std::vector<std::string>shapeNamesMiddle2{"cross", "crossSmooth", "triangle", "triangleWithin", "star", "box"};
        std::vector<std::string>shapeNamesMiddle3{"cross", "crossSmooth", "box", "boxWithin", "star", "triangle"};
        //Hard
        std::vector<std::string>shapeNamesHard1{"box", "boxWithin", "cross", "circle", "crossSmooth", "star", "triangle"};
        std::vector<std::string>shapeNamesHard2{"box", "boxWithin", "cross", "circle", "crossSmooth", "star", "triangle", "triangleWithin"};
        
        std::vector<std::string>shapeNames;
        // Select shape names based on difficulty
        switch (currentDifficulty)
        {
            case Difficulty::Easy:
                switch (getRandomNumber(0, 2))
                {
                    case 0: shapeNames = shapeNamesEasy1; break;
                    case 1: shapeNames = shapeNamesEasy2; break;
                    case 2: shapeNames = shapeNamesEasy3; break;
                }
                break;
            case Difficulty::Middle:
                switch (getRandomNumber(0, 2))
                {
                    case 0: shapeNames = shapeNamesMiddle1; break;
                    case 1: shapeNames = shapeNamesMiddle2; break;
                    case 2: shapeNames = shapeNamesMiddle3; break;
                }
                break;
            case Difficulty::Hard:
                switch (getRandomNumber(0, 1))
                {
                    case 0: shapeNames = shapeNamesHard1; break;
                    case 1: shapeNames = shapeNamesHard2; break;
                }
                break;
        }


        int shapeToFind = getRandomNumber(0,shapeNames.size()-1);
        std::string shapeToFindName = shapeNames[shapeToFind];
        shapeNames.erase(shapeNames.begin() + shapeToFind);
        shapeNames.resize(shapeNames.size());
        bool shapeToFindPlaced = false;

        
        for(int i = 0; i< entitiesToFill ; i++)
        {
            glm::vec3 pos(getRandomNumber(glm::floor(gameEnv->getCameraPos().x-gameEnv->getXHalf()), glm::floor(gameEnv->getCameraPos().x+gameEnv->getXHalf())), getRandomNumber(glm::floor(gameEnv->getCameraPos().y-gameEnv->getYHalf()), glm::floor(gameEnv->getCameraPos().y+gameEnv->getYHalf())),0);
            glm::vec3 scale(getRandomNumber(2,10)*0.1f);
            float rotZ(getRandomNumber(0,360));
            if(!gameEnv->getPhysicsEngine()->checkIfShellWouldCollide(pos,scale,rotZ))
            {
                if(shapeToFindPlaced == false)
                {
                    gameEnv->addEntity(std::make_unique<Shape>("ShapeToFindHere", pos,scale, rotZ, true, shapeToFindName));
                    auto randomEntity = gameEnv->getEntityFromName<Entity>("ShapeToFindHere");
                    randomEntity->addComponent(std::make_unique<PhysicsCollider>(randomEntity,0));
                    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("ShapeToFindHere","Physics"));
                    shapeToFindPlaced = true;
                    continue;
                }
                
                std::string name = random_string(4);
                gameEnv->addEntity(std::make_unique<Shape>(name, pos,scale, rotZ, true, shapeNames[getRandomNumber(0,shapeNames.size()-1)]));
                auto randomEntity = gameEnv->getEntityFromName<Entity>(name);
                randomEntity->addComponent(std::make_unique<PhysicsCollider>(randomEntity,getRandomNumber(0,1)));
                //dynamic_cast<PhysicsCollider*>(randomEntity->getComponent("Physics"))->setMass(getRandomNumber(1,10));
                gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>(name,"Physics"));
            }
        }
    }

    gameEnv->registerRepeatingFunction
    (
        [this]()
        {
            auto timer = gameEnv->getEntityFromName<UiElement>("Timer");
            if(gameEnv->getCurrentMouseCollider() && gameEnv->getCurrentMouseCollider()->getEntityThisIsAttachedTo()->getEntityName() == "ShapeToFindHere" && !this->shapeFound && gameEnv->getIfPressedAndHolding())
                this->shapeFound = true;
            this->timeToComplete+=gameEnv->getDeltaTime();
            timer->setTextToBeRenderd("Time: " + std::to_string((int)this->timeToComplete));
        },
        [this]() -> bool
        {
            if(this->shapeFound)
            {
                this->handleNextMinigame();
            }
            return this->shapeFound;
        }
    );
}

void MinigameManager::miniGameGoToPosition()
{
    std::vector<std::string>playerNames;
    std::vector<std::string>triggerNames;
    std::vector<std::string>levelNames;
    //Names based on difficulty
    switch (currentDifficulty)
    {
        case Difficulty::Easy:
            playerNames = {"Player1"};
            triggerNames = {"TriggerBox1"};
            levelNames = {"PosLevelEasy1", "PosLevelEasy2", "PosLevelEasy3"};
            break;
        case Difficulty::Middle:
            playerNames = {"Player1", "Player2"};
            triggerNames = {"TriggerBox1", "TriggerBox2"};
            levelNames = {"PosLevelMedium1", "PosLevelMedium2", "PosLevelMedium3"};
            break;
        case Difficulty::Hard:
            playerNames = {"Player1", "Player2", "Player3"};
            triggerNames = {"TriggerBox1", "TriggerBox2", "TriggerBox3"};
            levelNames = {"PosLevelHard1", "PosLevelHard2", "PosLevelHard3"};
            break;
        
        default:
            break;
    }
    gameEnv->getSceneManager().loadLevel(levelNames[roundsPlayed%3], gameEnv->getEntities(), gameEnv->getPhysicsEngine());
    gameEnv->registerRepeatingFunction(
        [this, playerNames, triggerNames]() 
        {
            //Timer just for testing
            auto timer = this->gameEnv->getEntityFromName<UiElement>("Timer");
            if(!timer)
            {
                gameEnv->addEntity(std::make_unique<UiElement>("Timer",glm::vec3(3.3,24.2,0),glm::vec3(1),0,"Time: 0","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
                timer = gameEnv->getEntityFromName<UiElement>("Timer");
            }
            timer->setTextToBeRenderd("Time: " + std::to_string((int)this->timeToComplete));
            int i = 0;
            for(auto& triggerBoxes:triggerNames)
            {
                //Rotation
                auto refTrigger = this->gameEnv->getEntityFromName<Entity>(triggerBoxes);
                refTrigger->setZRotation((refTrigger->getRotation()+10*this->gameEnv->getDeltaTime()));  
                //Color
                if(this->gameEnv->getPhysicsEngine()->checkTriggerColliderCollision(playerNames[i], triggerBoxes))
                    refTrigger->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                else 
                    refTrigger->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                i++;  
            }
            this->timeToComplete+=this->gameEnv->getDeltaTime();
        },
        [this, playerNames, triggerNames]() -> bool 
        {
            //Check if all players are in the trigger boxes
            int i = 0;
            for(auto& playerName:playerNames)
            {
                if(!this->gameEnv->getPhysicsEngine()->checkTriggerColliderCollision(playerName, triggerNames[i]))
                    return false;
                i++;
            }
            this->handleNextMinigame();
            return true;
        }
    );
}

void MinigameManager::miniGameCatch()
{
    std::vector<std::string>spawnerNames;
    std::vector<std::string>levelNames;
    int numberOfShapesToSpawn = 0;
    
    //Names based on difficulty
    switch (currentDifficulty)
    {
        case Difficulty::Easy:
            spawnerNames = {"Spawner1", "Spawner2", "Spawner3", "Spawner4"};
            levelNames = {"BalanceLevelEasy1", "BalanceLevelEasy2", "BalanceLevelEasy3"};
            numberOfShapesToSpawn = 20;
            break;
        case Difficulty::Middle:
            spawnerNames = {"Spawner1", "Spawner2", "Spawner3", "Spawner4", "Spawner5"};
            levelNames = {"BalanceLevelMedium1", "BalanceLevelMedium2", "BalanceLevelMedium3"};
            numberOfShapesToSpawn = 25;
            break;
        case Difficulty::Hard:
            spawnerNames = {"Spawner1", "Spawner2", "Spawner3", "Spawner4", "Spawner5", "Spawner6"};
            levelNames = {"BalanceLevelHard1", "BalanceLevelHard2", "BalanceLevelHard3"};
            numberOfShapesToSpawn = 30;
            break;
        
        default:
            break;
    }
    gameEnv->getSceneManager().loadLevel(levelNames[roundsPlayed%3], gameEnv->getEntities(), gameEnv->getPhysicsEngine());
    //Status
    gameEnv->addEntity(std::make_unique<UiElement>("Status",glm::vec3(2,24,0),glm::vec3(1),0,"0/20","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
    //Prep
    for(auto& entry:spawnerNames)
        gameEnv->getEntityFromName<Entity>(entry)->setDontDraw(true);
    gameEnv->getEntityFromName<PlayerShape>("Player")->velocity = 5.f;
    //GameBalance
    if(currentDifficulty == Difficulty::Easy&& roundsPlayed == 2)
        spawnInterval = 0.075f;
    if(currentDifficulty == Difficulty::Middle)
        spawnInterval = 0.05f;
    if(roundsPlayed == 5)
        spawnInterval = 0.06f;
    if(currentDifficulty == Difficulty::Hard)
        spawnInterval = 0.04f;
    if(roundsPlayed == 7)
        gameEnv->getEntityFromName<PlayerShape>("Player")->velocity = 7.f;
    if(roundsPlayed == 8)
        gameEnv->getEntityFromName<PlayerShape>("Player")->velocity = 5.f;  
    
    //Logic
    gameEnv->registerRepeatingFunction
    (
        [this, spawnerNames, numberOfShapesToSpawn]() 
        {
            //Every x time interval spawn a shape at a spawner location +y so that it falls down
            if(this->timeElapsed > numberOfShapesToSpawn*spawnInterval && this->shapesSpawned < numberOfShapesToSpawn)
            {
                //Logic that spawns shapes
                // "Good" shapes in green that need to be catched
                    //Key in table 1
                // "Bad" shapes in red that need to be avoided
                    //Key in table 0 
                if(this->currentDifficulty == Difficulty::Middle)
                    this->positionAlternation = getRandomNumber(-5,5)/10;
                
                
                int good = getRandomNumber(0,10);
                if(good > this->bias) //Bias towards good shapes
                    good = 1;
                else
                    good = 0;
                auto spawner = gameEnv->getEntityFromName<Entity>(spawnerNames[getRandomNumber(0,spawnerNames.size()-1)]);
                std::string entityName = std::string("ShapeToCatch") + std::to_string(this->shapesSpawned);
                auto shape = std::make_unique<Shape>(entityName, glm::vec3(spawner->getPosition().x+positionAlternation,spawner->getPosition().y+5,0),glm::vec3(1), getRandomNumber(0,360), true, "circle");
                shape->addComponent(std::make_unique<PhysicsCollider>(shape.get(),0));
                if(good)
                    shape->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                else
                    shape->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                gameEnv->addEntity(std::move(shape));
                this->gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>(entityName,"Physics"));
                
                if(good)
                    this->activeShapesMap[1].push_back(gameEnv->getEntityFromName<Entity>(entityName));
                else
                    this->activeShapesMap[0].push_back(gameEnv->getEntityFromName<Entity>(entityName));

                this->shapesSpawned++;
                this->timeElapsed = 0;
            }
            
            for (auto& [key, shapes] : this->activeShapesMap) 
            {
                for (auto& shape : shapes) 
                {
                    if (key == 1) 
                    {  // Good shapes
                        if (this->gameEnv->getPhysicsEngine()->checkColliderPlayerCollision(shape->getEntityName())) {
                            // Handle good shape collision
                            this->gameEnv->deleteEntityFromName(shape->getEntityName());
                            shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
                            shapesHandeldCorrectly++;
                            //color
                            this->gameEnv->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                            this->gameEnv->registerFunctionToExecuteWhen
                            (
                                0.5f, 
                                [this]
                                {
                                    if(this->gameEnv->getEntityFromName<PlayerShape>("Player"))
                                        this->gameEnv->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
                                }
                            );
                            continue;
                        }
                    }
                    else if (key == 0) 
                    {  // Bad shapes
                        if (this->gameEnv->getPhysicsEngine()->checkColliderPlayerCollision(shape->getEntityName())) 
                        {
                            this->gameEnv->deleteEntityFromName(shape->getEntityName());
                            shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
                            shapesHandeldCorrectly--;
                            //color
                            this->gameEnv->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                            this->gameEnv->registerFunctionToExecuteWhen
                            (
                                0.5f, 
                                [this]
                                {
                                    if(this->gameEnv->getEntityFromName<PlayerShape>("Player"))
                                        this->gameEnv->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
                                }
                            );
                            continue;
                        }
                    }

                    // Common shape handling (falling off screen)
                    if (shape->getPosition().y < 0) 
                    {
                        if(key == 1)
                            shapesHandeldCorrectly--;
                        if(key == 0)
                            shapesHandeldCorrectly++;
                        
                        this->gameEnv->deleteEntityFromName(shape->getEntityName());
                        shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
                    }
                }
            }
            
            //Status update
            if(shapesHandeldCorrectly < 0)
                shapesHandeldCorrectly = 0;
            auto status = gameEnv->getEntityFromName<UiElement>("Status");
            status->setTextToBeRenderd(std::to_string(this->shapesHandeldCorrectly) + "/" + std::to_string(numberOfShapesToSpawn));
            //Update
            this->timeElapsed+=this->gameEnv->getDeltaTime();
            this->timeToComplete+=this->gameEnv->getDeltaTime();
        },
        [this, numberOfShapesToSpawn]() -> bool 
        {
            if(this->shapesSpawned >= numberOfShapesToSpawn && this->activeShapesMap[0].size() == 0 && this->activeShapesMap[1].size() == 0)
            {
                this->handleNextMinigame();
                return true;
            }  
            return false;
        }
    );
}

void MinigameManager::startSunwaveGame()
{
    // Available minigames (excluding Sunwave)
    std::vector<MinigameType> availableTypes = 
    {
        MinigameType::FindShape,
        MinigameType::GoToPosition,
        MinigameType::Catch
    };

    // Create 3 pairs of 3 minigames
    std::vector<std::vector<MinigameType>> gamePairs;
    for(int pair = 0; pair < 3; pair++) 
    {
        std::vector<MinigameType> currentPair;
        auto shuffledTypes = availableTypes;
        
        // Shuffle available types for this pair
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(shuffledTypes.begin(), shuffledTypes.end(), gen);
        
        // Take all 3 types for this pair
        for(int i = 0; i < 3; i++) {
            currentPair.push_back(shuffledTypes[i]);
        }
        
        gamePairs.push_back(currentPair);
    };
    // Shuffle the pairs themselves
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(gamePairs.begin(), gamePairs.end(), gen);

    // Store the sequence (flatten the pairs into single sequence)
    minigameSequence.clear();
    for(const auto& pair : gamePairs) 
        for(const auto& game : pair) 
            minigameSequence.push_back(game);
        
    startMinigame(minigameSequence[0]);

}

void MinigameManager::resetMinigameVariabels()
{
    currentMinigame = MinigameType::None;
    currentDifficulty = Difficulty::Easy;
    roundsPlayed = 0;
    timeElapsed = 0.0f;
    shapeFound = false;
    entitiesToFill = 60;
    timeToComplete = 0.f;
    shapesSpawned = 0;
    shapesHandeldCorrectly = 0;
    bias = 2;
    spawnInterval = 0.05f;
    positionAlternation = 0.1f;
    activeShapesMap.clear();
    minigameSequence.clear();
}

void MinigameManager::startMinigame(MinigameType type)
{
    currentMinigame = type;
    switch (type)
    {
    case MinigameType::FindShape:
        miniGameFindShape();
        break;
    case MinigameType::GoToPosition:
        miniGameGoToPosition();
        break;
    case MinigameType::Catch:
        miniGameCatch();
        break;
    case MinigameType::Sunwave:
        startSunwaveGame();
        break;
    default:
        break;
    }
}

std::string MinigameManager::difficultyToString(Difficulty &difficulty)
{
    switch (difficulty)
    {
        case Difficulty::Easy: return "Easy";
        case Difficulty::Middle: return "Middle";
        case Difficulty::Hard: return "Hard";
        default: return "Unknown";
    }
}
