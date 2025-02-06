#include "MinigameManager.h"
#include "../GameEnvironment.h"
#include "../util/HelperFunctions.h"
#include <algorithm>
#include <filesystem>

void MinigameManager::handleNextMinigame()
{
    gameEnv->resetMouseStates();
    //This will called when a minigame is finished
    roundsPlayed++;
    //Difficulty increase based on rounds played
    if(roundsPlayed > 2)
        currentDifficulty = Difficulty::Middle;
    if(roundsPlayed > 5)
        currentDifficulty = Difficulty::Hard;
    if(roundsPlayed > 8)
    {
        loadEndCard();
        return;
    }
    
    //Blend for next minigame if sunwavemode is active
    if(!minigameSequence.empty()) //Sunwave
    {
        if(minigameSequence[roundsPlayed] == MinigameType::Catch)
        {
            this->shapesSpawned = 0;
            this->shapesHandeldCorrectly = 0;
        }
        
        if(currentMinigame == MinigameType::FindShape)
        {
            gameEnv->getPhysicsEngine()->setIsHalting(false);
            gameEnv->deleteEntityFromName("WallBottom");
            currentMinigame = minigameSequence[roundsPlayed];
            gameEnv->registerFunctionToExecuteWhen(4.f,[this]() {this->blendTheNextGame();});
            return;
        }
        currentMinigame = minigameSequence[roundsPlayed];
        blendTheNextGame();
        return;
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

void MinigameManager::handleHighscore()
{
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/minigames/highscores/highscores.json");

    nlohmann::json highscores;
    if(std::filesystem::exists(srcPath)) {
        std::ifstream input(srcPath);
        if(input.is_open()) {
            input >> highscores;
            input.close();
        }
    } else 
    {
        highscores = nlohmann::json{
            {"Sunwave", nlohmann::json::array()},
            {"Go!", nlohmann::json::array()},
            {"Catch!", nlohmann::json::array()},
            {"Shape?", nlohmann::json::array()}
        };
    }

    std::string gameType = gameTypeToString(currentMinigame);
    bool shouldUpdate = false;

    if(currentMinigame == MinigameType::Catch) 
    {
        // For Catch, compare shapesHandeldCorrectlyFull
        if(highscores[gameType].empty() || 
           shapesHandeldCorrectlyFull > highscores[gameType][0]["Shapes"].get<int>()) 
        {
            highscores[gameType] = nlohmann::json::array();
            highscores[gameType].push_back({
                {"Shapes", shapesHandeldCorrectlyFull}
            });
            shouldUpdate = true;
        }
    }
    else if(currentMinigame == MinigameType::Sunwave) 
    {
        // For Sunwave, compare both time and shapes
        if(highscores[gameType].empty() || 
           (timeToComplete < highscores[gameType][0]["Time"].get<float>() && 
            shapesHandeldCorrectlyFull > highscores[gameType][0]["Shapes"].get<int>())) {
            highscores[gameType] = nlohmann::json::array();
            highscores[gameType].push_back({
                {"Time", timeToComplete},
                {"Shapes", shapesHandeldCorrectlyFull}
            });
            shouldUpdate = true;
        }
    }
    else 
    {
        // For other games, compare time
        if(highscores[gameType].empty() || 
           timeToComplete < highscores[gameType][0]["Time"].get<float>()) {
            highscores[gameType] = nlohmann::json::array();
            highscores[gameType].push_back({
                {"Time", timeToComplete}
            });
            shouldUpdate = true;
        }
    }
    
    if(shouldUpdate) 
    {
        std::ofstream file(srcPath);
        file << highscores.dump(4);
    }
}

void MinigameManager::loadEndCard()
{
    gameEnv->setHoverOverEffect(true);
    gameEnv->prepareForLevelChange();
    gameEnv->getSceneManager().loadLevel("Endcard", gameEnv->getEntities(), gameEnv->getPhysicsEngine());
    
    auto backToMenuButton = gameEnv->getEntityFromName<UiElement>("BackToMenu");
    auto yourScoreText = gameEnv->getEntityFromName<UiElement>("YScoreText");
    auto prevHighScoreText = gameEnv->getEntityFromName<UiElement>("HScoreText");
    auto banner = gameEnv->getEntityFromName<UiElement>("Banner");

    if(!minigameSequence.empty())
        currentMinigame = MinigameType::Sunwave;

    // Read previous highscores
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/minigames/highscores/highscores.json");

    nlohmann::json highscores;
    bool isNewHighscore = false;
    if(std::filesystem::exists(srcPath)) {
        std::ifstream input(srcPath);
        if(input.is_open()) {
            input >> highscores;
            input.close();
        }
    }

    std::string gameType = gameTypeToString(currentMinigame);
    std::string highscoreText = "No highscore yet";

    if(!highscores[gameType].empty()) 
    {
        if(currentMinigame == MinigameType::Sunwave) 
            highscoreText = std::to_string((int)highscores[gameType][0]["Time"].get<float>()) + 
                           "s, " + std::to_string(highscores[gameType][0]["Shapes"].get<int>()) + "/75";
        else if(currentMinigame == MinigameType::Catch) 
            highscoreText = std::to_string(highscores[gameType][0]["Shapes"].get<int>()) + "/225";
        else 
            highscoreText = std::to_string((int)highscores[gameType][0]["Time"].get<float>()) + "s";    
    }

    // Check if new highscore based on game type
    if(currentMinigame == MinigameType::Catch) 
        isNewHighscore = highscores[gameType].empty() || 
                        shapesHandeldCorrectlyFull > highscores[gameType][0]["Shapes"].get<int>();
    else if(currentMinigame == MinigameType::Sunwave) 
        isNewHighscore = highscores[gameType].empty() || 
                        (timeToComplete < highscores[gameType][0]["Time"].get<float>() && 
                         shapesHandeldCorrectlyFull > highscores[gameType][0]["Shapes"].get<int>());
    else 
        isNewHighscore = highscores[gameType].empty() || 
                        timeToComplete < highscores[gameType][0]["Time"].get<float>();
     // Set color based on highscore achievement
    yourScoreText->setTextColor( 
        isNewHighscore ? glm::vec4(0, 1, 0, 1) : glm::vec4(1, 0, 0, 1));
    
    prevHighScoreText->setTextToBeRenderd(highscoreText);
    
    if(!minigameSequence.empty())
        currentMinigame = MinigameType::Sunwave;
    if(currentMinigame == MinigameType::Sunwave)
        yourScoreText->setTextToBeRenderd(std::to_string((int)timeToComplete) + "s, " + std::to_string(shapesHandeldCorrectlyFull) + "/75");
    else if(currentMinigame == MinigameType::Catch)
        yourScoreText->setTextToBeRenderd(std::to_string(shapesHandeldCorrectlyFull) + "/225"); 
    else
        yourScoreText->setTextToBeRenderd(std::to_string((int)timeToComplete)+ "s");

    if(calledMinigame == MinigameType::Sunwave)
        banner->setTextToBeRenderd(std::string("Sunwave complete!"));
    else
        banner->setTextToBeRenderd(gameTypeToString(currentMinigame) + " complete!");

    //Button
    backToMenuButton->setOnClick([this]
    {
        this->handleHighscore();
        this->gameEnv->resetMouseStates();
        this->gameEnv->setMouseEntityManipulation(false);
        this->gameEnv->setHoverOverEffect(true);
        this->resetMinigameVariabels();
        this->gameEnv->loadMenu();
    });
}

void MinigameManager::miniGameFindShape()
{
    if(calledMinigame != MinigameType::Sunwave)
        gameEnv->playMusicByName("shape");
    gameEnv->prepareForLevelChange();
    gameEnv->getPhysicsEngine()->setIsHalting(true);
    shapeFound = false;
    gameEnv->setHoverOverEffect(true);
    gameEnv->setMouseEntityManipulation(true);

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
                return true;
            }
            return this->shapeFound;
        }
    );
}

void MinigameManager::miniGameGoToPosition()
{
    if(calledMinigame != MinigameType::Sunwave)
        gameEnv->playMusicByName("go");
    gameEnv->prepareForLevelChange();
    std::vector<std::string>playerNames;
    std::vector<std::string>triggerNames;
    std::vector<std::string>levelNames;
    gameEnv->setHoverOverEffect(false);
    
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
    if(calledMinigame != MinigameType::Sunwave)
        gameEnv->playMusicByName("catch");
    gameEnv->prepareForLevelChange();
    std::vector<std::string>spawnerNames;
    std::vector<std::string>levelNames;
    int numberOfShapesToSpawn = 0;
    gameEnv->setHoverOverEffect(false);
    
    
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
    gameEnv->getPhysicsEngine()->setIsHalting(false);
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
                                    if(this->gameEnv->getEntityFromName<PlayerShape>("Player") != nullptr)
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
                                    if(this->gameEnv->getEntityFromName<PlayerShape>("Player")!= nullptr)
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
                shapesHandeldCorrectlyFull += shapesHandeldCorrectly;
                this->handleNextMinigame();
                return true;
            }  
            return false;
        }
    );
}

void MinigameManager::startSunwaveGame()
{
    gameEnv->playMusicByName("sunwave");
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
    
    blendTheNextGame();

}

void MinigameManager::blendTheNextGame()
{
    gameEnv->resetMouseStates();
    gameEnv->getSceneManager().loadLevel("Blend", gameEnv->getEntities(), gameEnv->getPhysicsEngine());
    gameEnv->setMouseEntityManipulation(false);
    gameEnv->setHoverOverEffect(true);

    //Text
    auto upGameText = gameEnv->getEntityFromName<UiElement>("upGameText");
    upGameText->setTextToBeRenderd(gameTypeToString(minigameSequence[roundsPlayed]));
    auto diffShowText = gameEnv->getEntityFromName<UiElement>("diffShowText");
    diffShowText->setTextToBeRenderd(difficultyToString(currentDifficulty));
    auto controlShowText = gameEnv->getEntityFromName<UiElement>("controlsShowText");
    controlShowText->setTextToBeRenderd(typeToControlsString(minigameSequence[roundsPlayed]));

    //Corrections
    if(minigameSequence[roundsPlayed] == MinigameType::GoToPosition)
    {
        glm::vec3 adjustedPos;
        adjustedPos = controlShowText->getPosition();
        adjustedPos.x -=2.7f;
        controlShowText->setPosition(adjustedPos);
        adjustedPos = upGameText->getPosition();
        adjustedPos.x -=1.5f;
        upGameText->setPosition(adjustedPos);
        createGoToPositionExplanation();
    }
    else if(minigameSequence[roundsPlayed] == MinigameType::Catch)
    {
        glm::vec3 adjustedPos;
        adjustedPos = controlShowText->getPosition();
        adjustedPos.x -=2.7f;
        controlShowText->setPosition(adjustedPos);
        adjustedPos = upGameText->getPosition();
        adjustedPos.x -=0.6f;
        upGameText->setPosition(adjustedPos);
        createCatchExplanation();
    }
    else if (minigameSequence[roundsPlayed] == MinigameType::FindShape)
    {
        glm::vec3 adjustedPos;
        adjustedPos = upGameText->getPosition();
        adjustedPos.x -=0.3f;
        upGameText->setPosition(adjustedPos);
        createFindShapeExplanation();
    }
    

    //Buttons
    auto startButton = gameEnv->getEntityFromName<UiElement>("startButton");
    startButton->setOnClick([this]
    {
        if(!gameEnv->getGamePaused())
            this->startMinigame(this->minigameSequence[roundsPlayed]);
    });
    //Enter to start
    gameEnv->registerRepeatingFunction
    (
        [this]()
        {
        },
        [this]() -> bool
        {
            auto startButton = gameEnv->getEntityFromName<UiElement>("startButton");
            if(!startButton)
                return true;
            if(glfwGetKey(gameEnv->getCurrentWindow(), GLFW_KEY_ENTER) == GLFW_PRESS || glfwGetKey(gameEnv->getCurrentWindow(), GLFW_KEY_KP_ENTER) == GLFW_PRESS)
            {
                if(!gameEnv->getGamePaused())
                    this->startMinigame(this->minigameSequence[roundsPlayed]);
                return true;
            }
            return false;    
        }
    );
}

void MinigameManager::createFindShapeExplanation()
{
    gameEnv->setMouseEntityManipulation(true);
    std::vector<std::string>shapeNames{"box", "circle", "triangle"};
    std::string shapeToFindName = shapeNames[getRandomNumber(0,shapeNames.size()-1)];
    std::vector<int> xMinMax = {30, (int)(gameEnv->getXHalf()*2)-1};
    std::vector<int> yMinMax = {10, 23};
    glm::vec3 scale(getRandomNumber(2,10)*0.1f);
    std::vector<std::string> randomNames;
    auto shapeToFindExp = std::make_unique<Shape>("ShapeToFindExp", glm::vec3(getRandomNumber(xMinMax[0],xMinMax[1]),getRandomNumber(yMinMax[0],yMinMax[1]),0),scale, getRandomNumber(0,360), true, shapeToFindName);
    shapeToFindExp->addComponent(std::make_unique<PhysicsCollider>(shapeToFindExp.get(),1));
    gameEnv->addEntity(std::move(shapeToFindExp));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("ShapeToFindExp","Physics"));
    shapeNames.erase(std::remove(shapeNames.begin(), shapeNames.end(), shapeToFindName), shapeNames.end());
    for(int i = 0;i < 35;i++)
    {
        auto name = random_string(4);
        glm::vec3 pos(getRandomNumber(xMinMax[0],xMinMax[1]),getRandomNumber(yMinMax[0],yMinMax[1]),0);
        glm::vec3 scale(getRandomNumber(2,10)*0.1f);
        float rotZ(getRandomNumber(0,360));
        if(!gameEnv->getPhysicsEngine()->checkIfShellWouldCollide(pos,scale,rotZ))
        {
            gameEnv->addEntity(std::make_unique<Shape>(name, pos,scale, rotZ, true, shapeNames[getRandomNumber(0,shapeNames.size()-1)]));
            auto randomEntity = gameEnv->getEntityFromName<Entity>(name);
            randomEntity->addComponent(std::make_unique<PhysicsCollider>(randomEntity,1));
            gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>(name,"Physics"));
            randomNames.push_back(name);
        }
    }

    gameEnv->registerRepeatingFunction
    (
        [this, randomNames]()
        {
            auto shapeTofindExp = gameEnv->getEntityFromName<Entity>("ShapeToFindExp");
            if(!shapeTofindExp)
                return;
            if(!gameEnv->getCurrentMouseCollider())
                return;
            if(gameEnv->getCurrentMouseCollider()->getEntityThisIsAttachedTo()->getEntityName() == "ShapeToFindExp" && gameEnv->getIfPressedAndHolding())
            {
                gameEnv->getEntityFromName<Entity>("ShapeToFindExp")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                for(auto& name:randomNames)
                {
                    gameEnv->getEntityFromName<Entity>(name)->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                }
            }
            else          
            {
                gameEnv->getEntityFromName<Entity>("ShapeToFindExp")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
                for(auto& name:randomNames)
                {
                    gameEnv->getEntityFromName<Entity>(name)->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
                }
            }
        },
        [this]() -> bool
        {
            auto shapeTofindExp = gameEnv->getEntityFromName<Entity>("ShapeToFindExp");
            if(!shapeTofindExp)
            {
                gameEnv->setHoverOverColor(glm::vec4(1,1,1,1));
                return true;
            }
            return false;
        }
    );

    
}

void MinigameManager::createGoToPositionExplanation()
{
    auto wall1 = std::make_unique<Shape>("Wall1", glm::vec3(35,10,0),glm::vec3(5,0.5,1), 0.0f, true, "box");
    wall1->addComponent(std::make_unique<PhysicsCollider>(wall1.get(),1));
    gameEnv->addEntity(std::move(wall1));
    auto wall2 = std::make_unique<Shape>("Wall2", glm::vec3(30.f,15.f,0),glm::vec3(5,0.5,1), 90.f, true, "box");
    wall2->addComponent(std::make_unique<PhysicsCollider>(wall2.get(),1));
    gameEnv->addEntity(std::move(wall2));
    auto wall3 = std::make_unique<Shape>("Wall3", glm::vec3(40.f,15.f,0),glm::vec3(5,0.5,1), 90.f, true, "box");
    wall3->addComponent(std::make_unique<PhysicsCollider>(wall3.get(),1));
    gameEnv->addEntity(std::move(wall3));
    auto wall4 = std::make_unique<Shape>("Wall4", glm::vec3(35,20,0),glm::vec3(5,0.5,1), 0, true, "box");
    wall4->addComponent(std::make_unique<PhysicsCollider>(wall4.get(),1));
    gameEnv->addEntity(std::move(wall4));
    auto TriggerBoxExp = std::make_unique<Shape>("TriggerBoxExp", glm::vec3(38,18,0),glm::vec3(0.75f), 0.0f, true, "box");
    TriggerBoxExp->addComponent(std::make_unique<PhysicsCollider>(TriggerBoxExp.get(),1));
    gameEnv->addEntity(std::move(TriggerBoxExp));
    gameEnv->getComponentOfEntity<PhysicsCollider>("TriggerBoxExp","Physics")->setIsTrigger(true);
    auto player = std::make_unique<PlayerShape>("Player", glm::vec3(32,12,0),glm::vec3(1), 0, true, "box");
    player->addComponent(std::make_unique<PhysicsCollider>(player.get(),0));
    gameEnv->addEntity(std::move(player));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Wall1","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Wall2","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Wall3","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Wall4","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("TriggerBoxExp","Physics"));
    

    //ColorChange logic
    gameEnv->registerRepeatingFunction
    (
        [this]()
        {
            auto player = gameEnv->getEntityFromName<PlayerShape>("Player");
            auto TriggerBoxExp = gameEnv->getEntityFromName<Entity>("TriggerBoxExp");
            if(!TriggerBoxExp)
                return;
            //rotate Triggerbox
            TriggerBoxExp->setZRotation(TriggerBoxExp->getRotation()+10*gameEnv->getDeltaTime());
            if(gameEnv->getPhysicsEngine()->checkTriggerColliderCollision("Player", "TriggerBoxExp"))
            {
                TriggerBoxExp->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                gameEnv->registerFunctionToExecuteWhen
                (
                    0.5f, 
                    [this]
                    {
                        auto triggerBoxExp = gameEnv->getEntityFromName<Entity>("TriggerBoxExp");
                        if(!triggerBoxExp)
                            return;
                        triggerBoxExp->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                    }
                );
            }
            else
            {
                TriggerBoxExp->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                gameEnv->registerFunctionToExecuteWhen
                (
                    0.5f, 
                    [this]
                    {
                        auto triggerBoxExp = gameEnv->getEntityFromName<Entity>("TriggerBoxExp");
                        if(!triggerBoxExp)
                            return;
                        triggerBoxExp->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
                    }
                );
            }
        },
        [this]() -> bool
        {
            auto TriggerBoxExp = gameEnv->getEntityFromName<Entity>("TriggerBoxExp");
            if(!TriggerBoxExp)
                return true;
            return false;
        }
    );
}

void MinigameManager::createCatchExplanation()
{
    auto Player = std::make_unique<PlayerShape>("Player", glm::vec3(35,10,0),glm::vec3(3,0.5,1), 0, true, "box");
    Player->addComponent(std::make_unique<PhysicsCollider>(Player.get(),0));
    gameEnv->addEntity(std::move(Player));
    gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics")->setLockY(true);
    auto wall1 = std::make_unique<Shape>("Wall1", glm::vec3(24,10,0),glm::vec3(0.5f,2,1), 0.0f, true, "box");
    wall1->addComponent(std::make_unique<PhysicsCollider>(wall1.get(),1));
    gameEnv->addEntity(std::move(wall1));
    auto wall2 = std::make_unique<Shape>("Wall2", glm::vec3(gameEnv->getXHalf()*2,10,0),glm::vec3(0.5f,2,1), 0.0f, true, "box");
    wall2->addComponent(std::make_unique<PhysicsCollider>(wall2.get(),1));
    gameEnv->addEntity(std::move(wall2));

    auto goodShape = std::make_unique<Shape>("GoodShapeExp", glm::vec3(40,25,0),glm::vec3(1), 0.0f, true, "circle");
    goodShape->addComponent(std::make_unique<PhysicsCollider>(goodShape.get(),0));
    goodShape->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
    gameEnv->addEntity(std::move(goodShape));

    auto badShape = std::make_unique<Shape>("BadShapeExp", glm::vec3(30,25,0),glm::vec3(1), 0.0f, true, "circle");
    badShape->addComponent(std::make_unique<PhysicsCollider>(badShape.get(),0));
    badShape->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
    gameEnv->addEntity(std::move(badShape));


    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Player","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Wall1","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("Wall2","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("GoodShapeExp","Physics"));
    gameEnv->getPhysicsEngine()->registerPhysicsCollider(gameEnv->getComponentOfEntity<PhysicsCollider>("BadShapeExp","Physics"));

    //Fall logic
    gameEnv->registerRepeatingFunction
    (
        [this]()
        {
            auto goodShape = gameEnv->getEntityFromName<Entity>("GoodShapeExp");
            auto badShape = gameEnv->getEntityFromName<Entity>("BadShapeExp");
            auto player = gameEnv->getEntityFromName<PlayerShape>("Player");
            if(!goodShape || !badShape)
                return;
            if(goodShape->getPosition().y < 5)
            {
                goodShape->setPosition(goodShape->getPosition()+glm::vec3(0,20,0));
                if(goodShape->getPosition().x <23.5)
                {
                    glm::vec3 newPos = goodShape->getPosition();
                    newPos.x+= 10;
                    goodShape->setPosition(newPos);
                }
                if(goodShape->getPosition().x >gameEnv->getXHalf()*2-1)
                {
                    glm::vec3 newPos = goodShape->getPosition();
                    newPos.x-= 10;
                    goodShape->setPosition(newPos);
                }
            }
            
            if(badShape->getPosition().y < 5)
            {
                badShape->setPosition(badShape->getPosition()+glm::vec3(0,20,0));
                if(badShape->getPosition().x <23.5)
                {
                    glm::vec3 newPos = badShape->getPosition();
                    newPos.x+= 10;
                    badShape->setPosition(newPos);
                }
                if(badShape->getPosition().x >gameEnv->getXHalf()*2-1)
                {
                    glm::vec3 newPos = badShape->getPosition();
                    newPos.x-= 10;
                    badShape->setPosition(newPos);
                }
            }

            if(gameEnv->getPhysicsEngine()->checkColliderPlayerCollision("GoodShapeExp"))
                gameEnv->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
            
            if(gameEnv->getPhysicsEngine()->checkColliderPlayerCollision("BadShapeExp"))
                gameEnv->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
            
            
        },
        [this]() -> bool
        {
            auto goodShape = gameEnv->getEntityFromName<Entity>("GoodShapeExp");
            auto badShape = gameEnv->getEntityFromName<Entity>("BadShapeExp");
            if(!goodShape || !badShape)
                return true;
            return false;
        }
    );

}

void MinigameManager::resetMinigameVariabels()
{
    currentMinigame = MinigameType::None;
    calledMinigame = MinigameType::None;
    currentDifficulty = Difficulty::Easy;
    roundsPlayed = 0;
    timeElapsed = 0.0f;
    shapeFound = false;
    entitiesToFill = 60;
    timeToComplete = 0.f;
    shapesSpawned = 0;
    shapesHandeldCorrectly = 0;
    shapesHandeldCorrectlyFull = 0;
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
        calledMinigame = MinigameType::Sunwave;
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

std::string MinigameManager::gameTypeToString(MinigameType &type)
{
    switch (type)
    {
        case MinigameType::FindShape : return "Shape?";
        case MinigameType::Catch : return "Catch!";
        case MinigameType::GoToPosition : return "Go!";
        case MinigameType::Sunwave : return "Sunwave";
        default: return "Unknown";
    }
}

std::string MinigameManager::typeToControlsString(MinigameType &type)
{
    switch (type)
    {
        case MinigameType::FindShape : return "Mouse and click";
        case MinigameType::Catch : return "A and D";
        case MinigameType::GoToPosition : return "W A S D";
        default: return "Unknown";
    }
}
