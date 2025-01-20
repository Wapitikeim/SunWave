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
    //MiniGameLogic?
    if(refColliderForMouseCurrent && refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getEntityName() == "ShapeToFindHere" && !shapeFound && pressedAndHoldingSomething)
    {
        shapeFound = true;
        physicsEngine->setIsHalting(false);
        deleteEntityFromName("WallBottom");
        registerFunctionToExecuteWhen(5.f,[this]() {this->initFindTheShape();});
    }

    
    //Physics Finish (to have a chance to diff between Cur<->Old in Mouse Update)
    if(refColliderForMouseCurrent != refColliderForMouseOld)
        refColliderForMouseOld = refColliderForMouseCurrent;
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
        refColliderForMouseCurrent = refCollider;
    
}

void GameEnvironment::mouseHoverOverEffect()
{
    //ChangeShaderByOverOverEffect---
    if(refColliderForMouseCurrent != nullptr && !pressedAndHoldingSomething)
        refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1));
    if((refColliderForMouseCurrent != refColliderForMouseOld) && (refColliderForMouseOld != nullptr))
        refColliderForMouseOld->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
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

    if(pressedAndHoldingSomething)
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
        if(gamePaused)
            return;
        gamePaused = true;
        if(!physicsEngine->getIsHalting())
        {
            physicsEngine->setIsHalting(true);
            physicsEngineWasActive = true;
        }
        else
            physicsEngineWasActive = false;
            
        
        addEntity(std::make_unique<UiElement>("Pause banner",glm::vec3(21,20,0),glm::vec3(2),0,"Pause","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
        addEntity(std::make_unique<UiElement>("Back to menu",glm::vec3(21,10,0),glm::vec3(2),0,"Back to menu","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
        addEntity(std::make_unique<UiElement>("Resume banner",glm::vec3(21,15,0),glm::vec3(2),0,"Resume","Open_Sans\\static\\OpenSans-Regular.ttf", 64));
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
            this->refColliderForMouseCurrent = nullptr;
            this->deleteEntityFromName("Resume banner");
        });

        auto backToMenuBanner = getEntityFromName<UiElement>("Back to menu");
        backToMenuBanner->addComponent(std::make_unique<PhysicsCollider>(backToMenuBanner,1));
        getComponentOfEntity<PhysicsCollider>("Back to menu","Physics")->setIsTrigger(true);
        physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>("Back to menu","Physics"));
        backToMenuBanner->setOnClick([this]
        {
            this->setGamePaused(false);
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
    gamePaused = true;
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
        this->getUiManager().setShowImGuiUI(true);
        this->setMouseEntityManipulation(true);
        this->getSceneManager().loadLevel("Default",this->getEntities(),this->getPhysicsEngine());
    });
    
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
        this->setMouseEntityManipulation(true);
        this->initFindTheShape();
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

void GameEnvironment::miniGameFindShape()
{
    prepareForLevelChange();

    physicsEngine->setIsHalting(true);

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
        std::vector<std::string>shapeNames{"box", "cross", "circle", "sTriangle"};
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
                randomEntity->addComponent(std::make_unique<PhysicsCollider>(randomEntity,0));
                //dynamic_cast<PhysicsCollider*>(randomEntity->getComponent("Physics"))->setMass(getRandomNumber(1,10));
                physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>(name,"Physics"));
            }
        }
    }
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
    for (auto it = repeatingFunctions.begin(); it != repeatingFunctions.end();)
    {
        // Execute the stored function
        it->function();

        // Check the stop condition
        if (it->stopCondition())
        {
            // Erase the struct from the vector and get a valid iterator to the next element
            it = repeatingFunctions.erase(it);
        }
        else
        {
            // Move to the next element if not erasing
            ++it;
        }
    }
}

const bool &GameEnvironment::getShapeFound()
{
    if(shapeFound)
    {
        std::cout << "It took: " << timeToComplete << "\n";
        timeToComplete = 0;
        entitiesToFill+=20;
    }
    return shapeFound;  
}

void GameEnvironment::incrementTimeToComplete()
{
    if(!gamePaused)
        timeToComplete+=deltaTime;
}

void GameEnvironment::initFindTheShape()
{
    shapeFound = false;
    registerRepeatingFunction(
        [this]() {
            this->incrementTimeToComplete();
        },
        [this]() -> bool {
            return this->getShapeFound();
        }
    );
    miniGameFindShape();
}

void GameEnvironment::registerFunctionToExecuteWhen(float whenFunctionShouldStart, std::function<void()> functionToExecute)
{
    funcExecute t;
    t.timer = whenFunctionShouldStart;
    t.function = functionToExecute;
    functionsToExecuteAfterTime.push_back(t);
    std::cout << "Function Registerd with timer: " << t.timer << "\n";
}

GameEnvironment::GameEnvironment()
    :window(glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SunWave")),
    ui(window,this)
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
}

void GameEnvironment::testing(const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    
}
