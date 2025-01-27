#include "GameEnvironment.h"
#include <bitset>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>

//Just a random string generator (https://stackoverflow.com/questions/440133/how-do-i-create-a-random-alpha-numeric-string-in-c)
std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

//Just a random Int generator (https://stackoverflow.com/questions/7560114/random-number-c-in-some-range)
int getRandomNumber(int min, int max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}

void GameEnvironment::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
    pitch =  89.0f;
    if(pitch < -89.0f)
    pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void GameEnvironment::mouseUpdate()
{
    mouseCursorPositionUpdate();
    mouseClickUpdate();
    mousePhysicsUpdate();
    mouseHoverOverEffect();
    mouseClickLogic();
    if(entityManipulationThroughMouse)
        mouseEntityManipulationLogic();
    
    //Click Finish
    if (currentMouseLeftButtonState!= lastMouseLeftButtonState)
        lastMouseLeftButtonState = currentMouseLeftButtonState;

}

void GameEnvironment::mouseCursorPositionUpdate()
{
    //MouseCursor Update
    float yPos = glm::abs(ImGui::GetMousePos().y-glfwPrep::getCurrentWindowHeight());
    float xPos = ImGui::GetMousePos().x;
    //float mouseX = (ImGui::GetMousePos().x*(xHalf*2))/glfwPrep::getCurrentWindowWidth();
    mouseX = (xPos*(glm::abs((xHalf*2))-glm::abs(whatCameraSeesBottomLeft.x)))/(float)glfwPrep::getCurrentWindowWidth();
    mouseY = (yPos*(glm::abs((yHalf*2))-glm::abs(whatCameraSeesBottomLeft.y)))/(float)glfwPrep::getCurrentWindowHeight();
}

void GameEnvironment::mousePhysicsUpdate()
{
    //Update the refMouse Collider
    if(physicsEngine->getPhysicsObjectsEmpty())
    {
        refColliderForMouseOld = nullptr;
        refColliderForMouseCurrent = nullptr;
        return;
    }
    float rot = 0;
    auto refCollider = physicsEngine->getFirstColliderShellCollidesWith(glm::vec3(mouseX,mouseY,0),glm::vec3(0.01f),rot);
    
    if((refCollider != refColliderForMouseCurrent) && !pressedAndHoldingSomething)
    {
        refColliderForMouseOld = refColliderForMouseCurrent;
        refColliderForMouseCurrent = refCollider;
    }
        
    
}

void GameEnvironment::mouseHoverOverEffect()
{
    //ChangeShaderByOverOverEffect---
    if(refColliderForMouseOld)
    {
        //UiElement
        if(refColliderForMouseOld->getEntityThisIsAttachedTo() == entityColorSaved && entityColorSaved->getEntityType() == "UiElement")
        {
            dynamic_cast<UiElement*>(refColliderForMouseOld->getEntityThisIsAttachedTo())->setTextColor(colorOfRefEntity);
            entityColorSaved = nullptr;
        }
        //Other
        else if(refColliderForMouseOld->getEntityThisIsAttachedTo() == entityColorSaved)
        {
            refColliderForMouseOld->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
            entityColorSaved = nullptr;
        }
    } 
    if(refColliderForMouseCurrent)
    {
        //UiElement
        if(refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getEntityType() == "UiElement" && entityColorSaved != refColliderForMouseCurrent->getEntityThisIsAttachedTo())
        {
            colorOfRefEntity = dynamic_cast<UiElement*>(refColliderForMouseCurrent->getEntityThisIsAttachedTo())->getTextColor();
            dynamic_cast<UiElement*>(refColliderForMouseCurrent->getEntityThisIsAttachedTo())->setTextColor(glm::vec4(0,0,0,1));
            entityColorSaved = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
        }
        //Other
        else if(entityColorSaved != refColliderForMouseCurrent->getEntityThisIsAttachedTo())
        {
            refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1));
            entityColorSaved = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
        }
    }
}

void GameEnvironment::mouseClickUpdate()
{
    //0 = Release
    //1 = Press
    currentMouseLeftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(currentMouseLeftButtonState && lastMouseLeftButtonState)
        mouseLeftClickHoldingDown = true;
    else
        mouseLeftClickHoldingDown = false;
    
    if(mouseLeftClickHoldingDown && refColliderForMouseCurrent)
        pressedAndHoldingSomething = true;
    else
        pressedAndHoldingSomething = false;
}

void GameEnvironment::mouseButtonClickInteractionLogic()
{
    //UIelement Buttons catch####
    if(currentMouseLeftButtonState && mouseLeftClickHoldingDown && refColliderForMouseCurrent)
    {
        UiElement* uiElement = dynamic_cast<UiElement*>(refColliderForMouseCurrent->getEntityThisIsAttachedTo());
        if (uiElement)
            uiElement->click();
    }
}

void GameEnvironment::mouseEntityManipulationLogic()
{
    //Rotation Logic
    if(pressedAndHoldingSomething && refColliderForMouseCurrent)
    {
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        {
            auto refEntity = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
            float rot = refEntity->getRotation();
            refEntity->setZRotation(rot += 40.0f*deltaTime);
            if(refEntity->getRotation() > 360)
                refEntity->setZRotation(0);
        }
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            auto refEntity = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
            float rot = refEntity->getRotation();
            refEntity->setZRotation(rot -= 40.0f*deltaTime);
            if(refEntity->getRotation() < 0)
                refEntity->setZRotation(360);
        }
        float SCALEFACTOR = 0.8f;
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            auto refEntity = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
            glm::vec3 scale = refEntity->getScale();
            scale.y +=SCALEFACTOR*deltaTime;
            refEntity->setScale(scale);
        }
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        {
            auto refEntity = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
            glm::vec3 scale = refEntity->getScale();
            scale.y -=SCALEFACTOR*deltaTime;
            refEntity->setScale(scale);
        }
        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
        {
            auto refEntity = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
            glm::vec3 scale = refEntity->getScale();
            scale.x +=SCALEFACTOR*deltaTime;
            refEntity->setScale(scale);
        }
        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
        {
            auto refEntity = refColliderForMouseCurrent->getEntityThisIsAttachedTo();
            glm::vec3 scale = refEntity->getScale();
            scale.x -=SCALEFACTOR*deltaTime;
            refEntity->setScale(scale);
        }

    }
    
}

void GameEnvironment::mouseClickLogic()
{
    if(!refColliderForMouseCurrent)
        return;

    mouseButtonClickInteractionLogic();
    //It could be that the click loads or level or stmth 
    if(!refColliderForMouseCurrent)
        return;

    refColliderForMouseCurrent->updateCornerPositions();
    physicsEngine->addColliderIntoHashTable(refColliderForMouseCurrent);
    if (currentMouseLeftButtonState!= lastMouseLeftButtonState)
    {
        if(!lastMouseLeftButtonState)
        {
            staticPrevRef = refColliderForMouseCurrent->getIsStatic(); 
            refColliderForMouseCurrent->setIsStatic(staticPrevRef);        
        }
        else
            refColliderForMouseCurrent->setIsStatic(staticPrevRef);  
    }

    
    if(pressedAndHoldingSomething&& entityManipulationThroughMouse)
    {      
        refColliderForMouseCurrent->setPos(glm::vec3(mouseX,mouseY,0));
        refColliderForMouseCurrent->update();
        physicsEngine->addColliderIntoHashTable(refColliderForMouseCurrent);
    }

}

void GameEnvironment::drawEntities()
{
    for(auto &entity: entities)
    {
        entity->draw();
    }
}

void GameEnvironment::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f;
}

void GameEnvironment::updateWorldTranslationInfo()
{
    yHalf = std::sin(glm::radians(fov/2)) * (cameraPos.z/glm::sin(glm::radians((180-fov/2-90))));
    xHalf = yHalf*(SCREEN_WIDTH/SCREEN_HEIGHT);
    whatCameraSeesBottomLeft = glm::vec3(cameraPos.x-xHalf, cameraPos.y-yHalf,0);
    whatCameraSeesTopLeft = glm::vec3(cameraPos.x-xHalf, cameraPos.y+yHalf,0);
    whatCameraSeesTopRight = glm::vec3(cameraPos.x+xHalf, cameraPos.y+yHalf,0);
    whatCameraSeesBottomRight = glm::vec3(cameraPos.x+xHalf, cameraPos.y-yHalf,0);
}

void GameEnvironment::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        if(inMenu || gamePaused)
            return;
        gamePaused = true;
        if(!physicsEngine->getIsHalting())
        {
            physicsEngine->setIsHalting(true);
            physicsEngineWasActive = true;
        }
        else
            physicsEngineWasActive = false;
            
        addEntity(std::make_unique<UiElement>("Pause banner",glm::vec3(21,20,0),glm::vec3(1),0,"Pause","Open_Sans\\static\\OpenSans-Regular.ttf", 94));
        addEntity(std::make_unique<UiElement>("Back to menu",glm::vec3(21,10,0),glm::vec3(1),0,"Back to menu","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
        addEntity(std::make_unique<UiElement>("Resume banner",glm::vec3(21,15,0),glm::vec3(1),0,"Resume","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
        auto resumeBanner = getEntityFromName<UiElement>("Resume banner");
        resumeBanner->addComponent(std::make_unique<PhysicsCollider>(resumeBanner,1));
        dynamic_cast<PhysicsCollider*>(resumeBanner->getComponent("Physics"))->setIsTrigger(true);
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("Resume banner","Physics"));
        resumeBanner->setOnClick([this]
        {
            this->setGamePaused(false);
            if(this->getIfPhysicsEngineWasActive())
                this->getPhysicsEngine()->setIsHalting(false);
            this->deleteEntityFromName("Pause banner");
            this->deleteEntityFromName("Back to menu");
            this->resetMouseStates();
            this->deleteEntityFromName("Resume banner");
            
        });

        auto backToMenuBanner = getEntityFromName<UiElement>("Back to menu");
        backToMenuBanner->addComponent(std::make_unique<PhysicsCollider>(backToMenuBanner,1));
        getComponentOfEntity<PhysicsCollider>("Back to menu","Physics")->setIsTrigger(true);
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("Back to menu","Physics"));
        backToMenuBanner->setOnClick([this]
        {
            this->setGamePaused(false);
            this->getPhysicsEngine()->setIsHalting(false);
            this->roundsPlayed = 0;
            this->timeToComplete = 0;
            this->gameDifficultyLevel = Difficulty::Easy;
            this->shapesSpawned = 0;
            this->shapesHandeldCorrectly = 0;
            this->spawnInterval = 0.05f;
            this->positionAlternation = 0.1f;
            this->activeShapesMap.clear();
            this->resetMouseStates();
            this->resetRegisterdFunctions();
            this->loadMenu();
        });


    }
}

void GameEnvironment::updateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

void GameEnvironment::update()
{
    for(auto &entity: entities) 
    {
        entity->update(window, deltaTime);
    }
}

void GameEnvironment::loadMenu()
{
    sceneManager.loadLevel("Main Menu", entities, getPhysicsEngine());
    inMenu = true;
    ui.setShowImGuiUI(false);
    entityManipulationThroughMouse = false;
    auto exitButton = getEntityFromName<UiElement>("Exit Button");
    exitButton->setOnClick([this]
    {
        glfwSetWindowShouldClose(this->getCurrentWindow(), GLFW_TRUE);
    });
    
    auto startButton = getEntityFromName<UiElement>("Start Button");
    startButton->setOnClick([this]
    {
        this->resetMouseStates();
        this->loadLevelSelector();
    });

    auto devMode = getEntityFromName<UiElement>("Dev Button");
    devMode->setOnClick([this]
    {
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->getUiManager().setShowImGuiUI(true);
        this->setMouseEntityManipulation(true);
        this->getSceneManager().loadLevel("Default",this->getEntities(),this->getPhysicsEngine());
    });

    //Shenanigens
    auto* cube = getEntityFromName<Shape>("Cube");
    auto* cubeCollider = dynamic_cast<PhysicsCollider*>(cube->getComponent("Physics"));
    cubeCollider->setIsStatic(false);
    registerRepeatingFunction(
        [this]() {
            if(this->getEntityFromName<Shape>("Cube") == nullptr)
                return;
            if(this->getEntityFromName<Shape>("Cube")->getPosition().x > 47)
            {
                glm::vec3 newPos = this->getEntityFromName<Shape>("Cube")->getPosition();
                newPos.x-=45;
                this->getEntityFromName<Shape>("Cube")->setPosition(newPos);
            }
            if(this->getEntityFromName<Shape>("Cube")->getPosition().x < 0)
            {
                glm::vec3 newPos = this->getEntityFromName<Shape>("Cube")->getPosition();
                newPos.x+=45;
                this->getEntityFromName<Shape>("Cube")->setPosition(newPos);
            }
            if(this->getEntityFromName<Shape>("Cube")->getPosition().y < -2)
            {
                glm::vec3 newPos = this->getEntityFromName<Shape>("Cube")->getPosition();
                newPos.y+=30;
                this->getEntityFromName<Shape>("Cube")->setPosition(newPos);
            }
            this->getEntityFromName<Shape>("Cube")->setZRotation((this->getEntityFromName<Shape>("Cube")->getRotation()+10*this->getDeltaTime()));  
        },
        [this]() -> bool {
            if(this->getEntityFromName<Shape>("Cube") == nullptr)
                return true;
            return false;
        }
    );

}

void GameEnvironment::loadLevelSelector()
{
    sceneManager.loadLevel("Level selector", entities, getPhysicsEngine());
    
    auto backToMenuButton = getEntityFromName<UiElement>("BtM");
    backToMenuButton->setOnClick([this]
    {
        this->resetMouseStates();
        this->resetRegisterdFunctions();
        this->loadMenu();
    });

    auto sfButton = getEntityFromName<UiElement>("SF");
    sfButton->setOnClick([this]
    {
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(true);
        this->miniGameFindShape(this->gameDifficultyLevel);
    });

    auto btsButton = getEntityFromName<UiElement>("BtS");
    btsButton->setOnClick([this]
    {
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(false);
        this->miniGameCatch(this->gameDifficultyLevel);
    });

    auto gopButton = getEntityFromName<UiElement>("GoP");
    gopButton->setOnClick([this]
    {
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(false);
        this->getPhysicsEngine()->setIsHalting(false);
        this->miniGameGoToPosition(this->gameDifficultyLevel); 
    });

}

void GameEnvironment::prepareForLevelChange()
{
    physicsEngine->clearPhysicsObjects();
    entities.clear();
    refColliderForMouseCurrent = nullptr;
    refColliderForMouseOld = nullptr;
}

void GameEnvironment::resetLevel()
{
    prepareForLevelChange();
    loadMenu();
}

void GameEnvironment::loadWallLevel()
{
    prepareForLevelChange();

    //Player
    addEntity(std::make_unique<PlayerShape>("Player", glm::vec3(5.f,5.f,0.0f), glm::vec3(1.f), 0.0f, true, "box"));
    auto player = getEntityFromName<Entity>("Player");
    player->addComponent(std::make_unique<PhysicsCollider>(player,0));
    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("Player","Physics"));

    //Entities Prep
    addEntity(std::make_unique<Shape>("WallBottom", glm::vec3(xHalf,0.5f,0.3f),glm::vec3(xHalf,1.f,1.0f), 0.0f, true, "box"));
    auto WallBottom = getEntityFromName<Entity>("WallBottom");
    WallBottom->addComponent(std::make_unique<PhysicsCollider>(WallBottom,1));
    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("WallBottom","Physics"));

    addEntity(std::make_unique<Shape>("WallTop", glm::vec3(xHalf,yHalf*2-0.5f,0.3f),glm::vec3(xHalf,1.f,1.0f), 0.0f, true, "box"));
    auto WallTop = getEntityFromName<Entity>("WallTop");
    WallTop->addComponent(std::make_unique<PhysicsCollider>(WallTop,1));
    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("WallTop","Physics"));

    addEntity(std::make_unique<Shape>("wallLeft", glm::vec3(0.5f,yHalf,0.3f),glm::vec3(1.0f,yHalf,1.0f), 0.0f, true, "box"));
    auto wallLeft = getEntityFromName<Entity>("wallLeft");
    wallLeft->addComponent(std::make_unique<PhysicsCollider>(wallLeft,1));
    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("wallLeft","Physics"));

    addEntity(std::make_unique<Shape>("wallRight", glm::vec3(xHalf*2-0.5f,yHalf,0.3f),glm::vec3(1.f,yHalf,1.0f), 0.0f, true, "box"));
    auto wallRight = getEntityFromName<Entity>("wallRight");
    wallRight->addComponent(std::make_unique<PhysicsCollider>(wallRight,1));
    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("wallRight","Physics"));
}

void GameEnvironment::miniGameFindShape(Difficulty difficulty)
{
    prepareForLevelChange();

    physicsEngine->setIsHalting(true);
    shapeFound = false;

    //Timer
    addEntity(std::make_unique<UiElement>("Timer",glm::vec3(3.3,24,0),glm::vec3(1),0,"Time: 0","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
    getEntityFromName<Entity>("Timer")->addComponent(std::make_unique<PhysicsCollider>(getEntityFromName<Entity>("Timer"),1));
    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("Timer","Physics"));
    //Walls
    {
        addEntity(std::make_unique<Shape>("WallBottom", glm::vec3(xHalf,-0.7f,0.3f),glm::vec3(xHalf,1.f,1.0f), 0.0f, true, "box"));
        auto WallBottom = getEntityFromName<Entity>("WallBottom");
        WallBottom->addComponent(std::make_unique<PhysicsCollider>(WallBottom,1));
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("WallBottom","Physics"));

        addEntity(std::make_unique<Shape>("WallTop", glm::vec3(xHalf,yHalf*2+0.7,0.3f),glm::vec3(xHalf,1.f,1.0f), 0.0f, true, "box"));
        auto WallTop = getEntityFromName<Entity>("WallTop");
        WallTop->addComponent(std::make_unique<PhysicsCollider>(WallTop,1));
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("WallTop","Physics"));

        addEntity(std::make_unique<Shape>("wallLeft", glm::vec3(-0.7f,yHalf,0.3f),glm::vec3(1.0f,yHalf,1.0f), 0.0f, true, "box"));
        auto wallLeft = getEntityFromName<Entity>("wallLeft");
        wallLeft->addComponent(std::make_unique<PhysicsCollider>(wallLeft,1));
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("wallLeft","Physics"));

        addEntity(std::make_unique<Shape>("wallRight", glm::vec3(xHalf*2+0.7f,yHalf,0.3f),glm::vec3(1.f,yHalf,1.0f), 0.0f, true, "box"));
        auto wallRight = getEntityFromName<Entity>("wallRight");
        wallRight->addComponent(std::make_unique<PhysicsCollider>(wallRight,1));
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("wallRight","Physics"));
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
        switch (difficulty)
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
            glm::vec3 pos(getRandomNumber(glm::floor(cameraPos.x-xHalf), glm::floor(cameraPos.x+xHalf)), getRandomNumber(glm::floor(cameraPos.y-yHalf), glm::floor(cameraPos.y+yHalf)),0);
            glm::vec3 scale(getRandomNumber(2,10)*0.1f);
            float rotZ(getRandomNumber(0,360));
            if(!physicsEngine->checkIfShellWouldCollide(pos,scale,rotZ))
            {
                if(shapeToFindPlaced == false)
                {
                    addEntity(std::make_unique<Shape>("ShapeToFindHere", pos,scale, rotZ, true, shapeToFindName));
                    auto randomEntity = getEntityFromName<Entity>("ShapeToFindHere");
                    randomEntity->addComponent(std::make_unique<PhysicsCollider>(randomEntity,0));
                    physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("ShapeToFindHere","Physics"));
                    shapeToFindPlaced = true;
                    continue;
                }
                
                std::string name = random_string(4);
                addEntity(std::make_unique<Shape>(name, pos,scale, rotZ, true, shapeNames[getRandomNumber(0,shapeNames.size()-1)]));
                auto randomEntity = getEntityFromName<Entity>(name);
                randomEntity->addComponent(std::make_unique<PhysicsCollider>(randomEntity,getRandomNumber(0,1)));
                //dynamic_cast<PhysicsCollider*>(randomEntity->getComponent("Physics"))->setMass(getRandomNumber(1,10));
                physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>(name,"Physics"));
            }
        }
    }
    
    registerRepeatingFunction
    (
        [this]()
        {
            auto timer = this->getEntityFromName<UiElement>("Timer");
            if(this->getCurrentMouseCollider() && this->getCurrentMouseCollider()->getEntityThisIsAttachedTo()->getEntityName() == "ShapeToFindHere" && !this->shapeFound && this->getIfPressedAndHolding())
                this->shapeFound = true;
            this->timeToComplete+=this->getDeltaTime();
            timer->setTextToBeRenderd("Time: " + std::to_string((int)this->timeToComplete));
        },
        [this]() -> bool
        {
            if(this->shapeFound)
            {
                this->roundsPlayed++;
                if(this->roundsPlayed > 2)
                    this->gameDifficultyLevel = Difficulty::Middle;
                if(this->roundsPlayed > 5)
                    this->gameDifficultyLevel = Difficulty::Hard;
                if(this->roundsPlayed > 8)
                {
                    this->gameDifficultyLevel = Difficulty::Easy;
                    this->roundsPlayed = 0;
                    this->timeToComplete = 0;
                    this->entitiesToFill = 60;
                    this->resetMouseStates();
                    this->setMouseEntityManipulation(false);
                    this->loadMenu();
                    return true;
                }
                switch (this->gameDifficultyLevel)
                {
                    case Difficulty::Easy:
                        this->entitiesToFill = getRandomNumber(80, 120);
                        break;
                    case Difficulty::Middle:
                        this->entitiesToFill = getRandomNumber(120, 160);
                        break;
                    case Difficulty::Hard:
                        this->entitiesToFill = getRandomNumber(180, 250);
                        break;
                };
                this->getPhysicsEngine()->setIsHalting(false);
                deleteEntityFromName("WallBottom");
                this->registerFunctionToExecuteWhen(4.f,[this]() {this->miniGameFindShape(this->gameDifficultyLevel);});
            }
            return this->shapeFound;
        }
    );

}

void GameEnvironment::miniGameGoToPosition(Difficulty difficulty)
{
    std::vector<std::string>playerNames;
    std::vector<std::string>triggerNames;
    std::vector<std::string>levelNames;
    //Names based on difficulty
    switch (difficulty)
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
    sceneManager.loadLevel(levelNames[roundsPlayed%3], entities, getPhysicsEngine());
    registerRepeatingFunction(
        [this, playerNames, triggerNames]() 
        {
            //Timer just for testing
            auto timer = this->getEntityFromName<UiElement>("Timer");
            if(!timer)
            {
                addEntity(std::make_unique<UiElement>("Timer",glm::vec3(3.3,24.2,0),glm::vec3(1),0,"Time: 0","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
                timer = getEntityFromName<UiElement>("Timer");
            }
            timer->setTextToBeRenderd("Time: " + std::to_string((int)this->timeToComplete));
            int i = 0;
            for(auto& triggerBoxes:triggerNames)
            {
                //Rotation
                auto refTrigger = this->getEntityFromName<Entity>(triggerBoxes);
                refTrigger->setZRotation((refTrigger->getRotation()+10*this->getDeltaTime()));  
                //Color
                if(this->getPhysicsEngine()->checkTriggerColliderCollision(playerNames[i], triggerBoxes))
                    refTrigger->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                else 
                    refTrigger->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                i++;  
            }
            this->timeToComplete+=this->getDeltaTime();
        },
        [this, playerNames, triggerNames]() -> bool 
        {
            //Check if all players are in the trigger boxes
            int i = 0;
            for(auto& playerName:playerNames)
            {
                if(!this->getPhysicsEngine()->checkTriggerColliderCollision(playerName, triggerNames[i]))
                    return false;
                i++;
            }
            roundsPlayed++;
            if(roundsPlayed > 2)
                this->gameDifficultyLevel = Difficulty::Middle;
            if(roundsPlayed > 5)
                this->gameDifficultyLevel = Difficulty::Hard;
            if(roundsPlayed > 8)
            {
                this->gameDifficultyLevel = Difficulty::Easy;
                this->roundsPlayed = 0;
                this->timeToComplete = 0;
                this->resetMouseStates();
                this->loadMenu();
                return true;
            }
            this->miniGameGoToPosition(this->gameDifficultyLevel);
            return true;
        }
    );


}

void GameEnvironment::miniGameCatch(Difficulty difficulty)
{
    std::vector<std::string>spawnerNames;
    std::vector<std::string>levelNames;
    int numberOfShapesToSpawn = 0;
    
    //Names based on difficulty
    switch (difficulty)
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
    sceneManager.loadLevel(levelNames[roundsPlayed%3], entities, getPhysicsEngine());
    //Status
    addEntity(std::make_unique<UiElement>("Status",glm::vec3(2,24,0),glm::vec3(1),0,"0/20","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
    //Prep
    for(auto& entry:spawnerNames)
        getEntityFromName<Entity>(entry)->setDontDraw(true);
    getEntityFromName<PlayerShape>("Player")->velocity = 5.f;
    //GameBalance
    if(difficulty == Difficulty::Easy&& roundsPlayed == 2)
        spawnInterval = 0.075f;
    if(difficulty == Difficulty::Middle)
        spawnInterval = 0.05f;
    if(roundsPlayed == 5)
        spawnInterval = 0.06f;
    if(difficulty == Difficulty::Hard)
        spawnInterval = 0.04f;
    if(roundsPlayed == 7)
        this->getEntityFromName<PlayerShape>("Player")->velocity = 7.f;
    if(roundsPlayed == 8)
        this->getEntityFromName<PlayerShape>("Player")->velocity = 5.f;  
    
    //Logic
    registerRepeatingFunction
    (
        [this, spawnerNames, numberOfShapesToSpawn]() 
        {
            //Every x time interval spawn a shape at a spawner location +y so that it falls down
            if(timeElapsed > numberOfShapesToSpawn*spawnInterval && this->shapesSpawned < numberOfShapesToSpawn)
            {
                //Logic that spawns shapes
                // "Good" shapes in green that need to be catched
                    //Key in table 1
                // "Bad" shapes in red that need to be avoided
                    //Key in table 0 
                if(gameDifficultyLevel == Difficulty::Middle)
                    this->positionAlternation = getRandomNumber(-5,5)/10;
                
                
                int good = getRandomNumber(0,10);
                if(good > this->bias) //Bias towards good shapes
                    good = 1;
                else
                    good = 0;
                auto spawner = getEntityFromName<Entity>(spawnerNames[getRandomNumber(0,spawnerNames.size()-1)]);
                std::string entityName = std::string("ShapeToCatch") + std::to_string(this->shapesSpawned);
                auto shape = std::make_unique<Shape>(entityName, glm::vec3(spawner->getPosition().x+positionAlternation,spawner->getPosition().y+5,0),glm::vec3(1), getRandomNumber(0,360), true, "circle");
                shape->addComponent(std::make_unique<PhysicsCollider>(shape.get(),0));
                if(good)
                    shape->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                else
                    shape->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                addEntity(std::move(shape));
                this->getPhysicsEngine()->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>(entityName,"Physics"));
                
                if(good)
                    this->activeShapesMap[1].push_back(getEntityFromName<Entity>(entityName));
                else
                    this->activeShapesMap[0].push_back(getEntityFromName<Entity>(entityName));

                this->shapesSpawned++;
                this->timeElapsed = 0;
            }
            
            for (auto& [key, shapes] : this->activeShapesMap) 
            {
                for (auto& shape : shapes) 
                {
                    if (key == 1) 
                    {  // Good shapes
                        if (this->getPhysicsEngine()->checkColliderPlayerCollision(shape->getEntityName())) {
                            // Handle good shape collision
                            this->deleteEntityFromName(shape->getEntityName());
                            shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
                            shapesHandeldCorrectly++;
                            //color
                            this->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,1,0,1));
                            this->registerFunctionToExecuteWhen
                            (
                                0.5f, 
                                [this]
                                {
                                    this->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
                                }
                            );
                            continue;
                        }
                    }
                    else if (key == 0) 
                    {  // Bad shapes
                        if (this->getPhysicsEngine()->checkColliderPlayerCollision(shape->getEntityName())) 
                        {
                            this->deleteEntityFromName(shape->getEntityName());
                            shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
                            shapesHandeldCorrectly--;
                            //color
                            this->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1,0,0,1));
                            this->registerFunctionToExecuteWhen
                            (
                                0.5f, 
                                [this]
                                {
                                    this->getEntityFromName<PlayerShape>("Player")->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
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
                        
                        this->deleteEntityFromName(shape->getEntityName());
                        shapes.erase(std::remove(shapes.begin(), shapes.end(), shape), shapes.end());
                    }
                }
            }
            
            //Status update
            if(shapesHandeldCorrectly < 0)
                shapesHandeldCorrectly = 0;
            auto status = getEntityFromName<UiElement>("Status");
            status->setTextToBeRenderd(std::to_string(this->shapesHandeldCorrectly) + "/" + std::to_string(numberOfShapesToSpawn));
            //Update
            this->timeElapsed+=this->getDeltaTime();
            this->timeToComplete+=this->getDeltaTime();
        },
        [this, numberOfShapesToSpawn]() -> bool 
        {
            if(this->shapesSpawned >= numberOfShapesToSpawn && this->activeShapesMap[0].size() == 0 && this->activeShapesMap[1].size() == 0)
            {
                this->roundsPlayed++;
                if(this->roundsPlayed > 2)
                    this->gameDifficultyLevel = Difficulty::Middle;
                if(this->roundsPlayed > 5)
                    this->gameDifficultyLevel = Difficulty::Hard;
                if(this->roundsPlayed > 8)
                {
                    this->gameDifficultyLevel = Difficulty::Easy;
                    this->roundsPlayed = 0;
                    this->timeToComplete = 0;
                    this->shapesSpawned = 0;
                    this->shapesHandeldCorrectly = 0;
                    this->spawnInterval = 0.05f;
                    this->positionAlternation = 0.1f;
                    this->activeShapesMap.clear();
                    this->resetMouseStates();
                    this->resetRegisterdFunctions();
                    this->loadMenu();
                    return true;
                }
                this->shapesSpawned = 0;
                this->shapesHandeldCorrectly = 0;
                this->miniGameCatch(this->gameDifficultyLevel);
                return true;
            }  
            return false;
        }
    );

}

void GameEnvironment::updateFunctionEvents()
{
    if(gamePaused)
        return;
    for(auto it = functionsToExecuteAfterTime.begin(); it != functionsToExecuteAfterTime.end();)
    {
        it->timer -= deltaTime;

        if (it->timer <= 0) 
        {
            // Execute the stored function
            it->function();

            // Erase the struct from the vector and get a valid iterator to the next element
            it = functionsToExecuteAfterTime.erase(it);
        } 
        else 
        {
            // Move to the next element if not erasing
            ++it;
        }
    }
}

void GameEnvironment::updateLoopingFunctions()
{
    if(gamePaused)
        return;
        
    for(auto it = loopingFunctions.begin(); it != loopingFunctions.end();) {
        // Execute the function
        it->function();
        
        // Decrease timer
        it->timer -= deltaTime;
        
        // Remove if timer is up
        if(it->timer <= 0) {
            it = loopingFunctions.erase(it);
        } else {
            ++it;
        }
    }
}

void GameEnvironment::registerRepeatingFunction(std::function<void()> functionToExecute, std::function<bool()> stopCondition)
{
    RepeatingFunction rf;
    rf.function = functionToExecute;
    rf.stopCondition = stopCondition;
    repeatingFunctions.push_back(rf);
}

void GameEnvironment::updateRepeatingFunctions()
{
    if(gamePaused)
        return;

    std::vector<size_t> indicesToRemove;
    
    // First pass: Execute functions and collect indices to remove
    for (size_t i = 0; i < repeatingFunctions.size(); i++)
    {
        repeatingFunctions[i].function();
        
        if (repeatingFunctions[i].stopCondition())
            indicesToRemove.push_back(i);
        
    }

    // Second pass: Remove functions from back to front
    for (auto it = indicesToRemove.rbegin(); it != indicesToRemove.rend(); ++it)
        repeatingFunctions.erase(repeatingFunctions.begin() + *it);
    
}

void GameEnvironment::registerFunctionToExecuteWhen(float whenFunctionShouldStart, std::function<void()> functionToExecute)
{
    funcExecute t;
    t.timer = whenFunctionShouldStart;
    t.function = functionToExecute;
    functionsToExecuteAfterTime.push_back(t);
    std::cout << "Function Registerd with timer: " << t.timer << "\n";
}

void GameEnvironment::registerLoopingFunctionUntil(std::function<void()> functionToExecute, float secondsToRun)
{
    LoopingFunction lf;
    lf.timer = secondsToRun;
    lf.function = functionToExecute;
    loopingFunctions.push_back(lf);
}

std::string GameEnvironment::difficultyToString(Difficulty difficulty)
{
    switch (difficulty)
    {
        case Difficulty::Easy: return "Easy";
        case Difficulty::Middle: return "Middle";
        case Difficulty::Hard: return "Hard";
        default: return "Unknown";
    }
}

GameEnvironment::GameEnvironment()
    : window(glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SunWave")),
      ui(window, this)
{   
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    //mousePosX = xpos;
    //mousePosY = abs(ypos-SCREEN_HEIGHT);
}

void GameEnvironment::run()
{
    physicsEngine = std::make_unique<PhysicsEngine>();
    
    //Camera Prep
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up); 
    Camera::setCurrentCameraView(view);//Prep if no Camera Flight active
    
    //Physics Engine Prep
    updateWorldTranslationInfo();
    physicsEngine->setcameraXHalf(xHalf);
    physicsEngine->setcameraYHalf(yHalf);

    //Mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    //glfwSetCursorPosCallback(window, mouse_callback);
    
    //Zoom
    //glfwSetScrollCallback(window, scroll_callback); 

    //Grid Feature
    InfiniteGrid grid(ShaderContainer("gridVertexShader.vert", "gridFragmentShader.frag"));


    //Load Level
    loadMenu();
 
    glfwMaximizeWindow(window);

    //Loop
    while (!glfwWindowShouldClose(window))
    {   
        //Input
        processInput(window);

        //glfwSetCursorPosCallback(window, cursor_position_callback);

        //Rendering
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        //"z" Buffer - depth testing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ui.prepFrames();
        

        //Physics pre Update
        physicsEngine->getInitialTransform(deltaTime);

        //FPS
        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - prevTime >= 1.)
        {
            ui.setImGuiFps(fps);
            fps = 0;
            prevTime = currentTime;
        }
        //Camera Update (Theoriactically)
        Camera::setCurrentCameraView(glm::lookAt(cameraPos, cameraPos + cameraFront, up));
        Camera::setCurrentCameraProjection(glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
        Camera::setCurrentCameraOrto(glm::ortho(0.0f, (float)glfwPrep::getCurrentWindowWidth(), 0.0f, (float)glfwPrep::getCurrentWindowHeight()));
        mouseUpdate();
        //GameLogic      
        updateFunctionEvents();
        updateRepeatingFunctions();

        //Grid First for transperent Textures ->They get cut off if they enter the -y plane though 
        update();
        if(ui.getShowGrid())
            grid.drawGrid(ui.getGridSize());
        //"Physics" Reset AFTER Update
        physicsEngine->updatePhysics(); 
        drawEntities();
        
        ui.draw();
        glfwSwapBuffers(window);
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }

    
}

void GameEnvironment::resetMouseStates()
{
    currentMouseLeftButtonState = false;
    mouseLeftClickHoldingDown = false;
    lastMouseLeftButtonState = false;
    pressedAndHoldingSomething = false;
    staticPrevRef = false;
    refColliderForMouseCurrent = nullptr;
    refColliderForMouseOld = nullptr;
}

void GameEnvironment::resetRegisterdFunctions()
{
    functionsToExecuteAfterTime.clear();
    repeatingFunctions.clear();
    loopingFunctions.clear();
}

void GameEnvironment::testing(const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    
}


