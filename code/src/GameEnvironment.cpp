#include "GameEnvironment.h"
#include <bitset>

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>

#include "util/helperFunctions.h"

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
    if(hoverOverEffect)
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
            refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",hoverOverColor);
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
            this->getPhysicsEngine()->tickrateOfSimulation = 150;
            this->getPhysicsEngine()->speedOfSimulation = 1;
            this->getMinigameManager()->resetMinigameVariabels();
            this->setHoverOverEffect(true);
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
    //Camera Update
    Camera::setCurrentCameraView(glm::lookAt(cameraPos, cameraPos + cameraFront, up));
    Camera::setCurrentCameraProjection(glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));
    Camera::setCurrentCameraOrto(glm::ortho(0.0f, (float)glfwPrep::getCurrentWindowWidth(), 0.0f, (float)glfwPrep::getCurrentWindowHeight()));
    
    //Mouse
    mouseUpdate();
    
    //Function logic      
    updateFunctionEvents();
    updateRepeatingFunctions();
    
    //Entities
    for(auto &entity: entities) 
    {
        entity->update(window, deltaTime);
    }

    //"Physics" Reset AFTER entity Update
    physicsEngine->updatePhysics(); 
}

void GameEnvironment::loadMenu()
{
    playMusicByName("menu");
    
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
    inLevelSelector = true;
    auto backToMenuButton = getEntityFromName<UiElement>("BtM");
    auto sfButton = getEntityFromName<UiElement>("SF");
    auto btsButton = getEntityFromName<UiElement>("BtS");
    auto gopButton = getEntityFromName<UiElement>("GoP");
    auto sunwaveButton = getEntityFromName<UiElement>("SunWaveStart");
    
    //Highscore Hover Over
    auto highScoreBanner = getEntityFromName<UiElement>("hBanner");
    highScoreBanner->setDontDraw(true);
    // Read highscores once
    std::filesystem::path srcPath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(srcPath);
    srcPath.append("src/minigames/highscores/highscores.json");
    nlohmann::json highscores;
    if(std::filesystem::exists(srcPath)) 
    {
        std::ifstream input(srcPath);
        if(input.is_open()) {
            input >> highscores;
            input.close();
        }
    }


    //Buttons
    backToMenuButton->setOnClick([this]
    {
        this->setInLevelSelector(false);
        this->resetMouseStates();
        this->resetRegisterdFunctions();
        this->loadMenu();
    });
    sfButton->setOnClick([this]
    {
        this->setInLevelSelector(false);
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(true);
        this->getMinigameManager()->startMinigame(MinigameType::FindShape);
    });
    btsButton->setOnClick([this]
    {
        this->setInLevelSelector(false);
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(false);
        this->getMinigameManager()->startMinigame(MinigameType::Catch);
    });
    gopButton->setOnClick([this]
    {
        this->setInLevelSelector(false);
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(false);
        this->getPhysicsEngine()->setIsHalting(false);
        this->getMinigameManager()->startMinigame(MinigameType::GoToPosition);
    });
    sunwaveButton->setOnClick([this]
    {
        this->setInLevelSelector(false);
        this->resetMouseStates();
        this->setGamePaused(false);
        this->setInMenu(false);
        this->setMouseEntityManipulation(false);
        this->getMinigameManager()->startMinigame(MinigameType::Sunwave);
    });

    // Register repeating function for hover highscore effect
    registerRepeatingFunction(
        [this, highscores, highScoreBanner]() {
            if(!inLevelSelector)
                return;
            if(!getCurrentMouseCollider()) {
                highScoreBanner->setDontDraw(true);
                return;
            }

            std::string entityName = getCurrentMouseCollider()->getEntityThisIsAttachedTo()->getEntityName();
            std::string highscoreText = "No highscore yet";

            if(entityName == "SF" && !highscores["Shape?"].empty()) 
            {
                highscoreText = "Highscore: " + std::to_string((int)highscores["Shape?"][0]["Time"].get<float>())+ "s";
                highScoreBanner->setDontDraw(false);
                glm::vec3 adjustedPos = getEntityFromName<UiElement>(entityName)->getPosition();
                adjustedPos.x += 12.5f;
                highScoreBanner->setPosition(adjustedPos);
            }
            else if(entityName == "GoP" && !highscores["Go!"].empty()) {
                highscoreText = "Highscore: " + std::to_string((int)highscores["Go!"][0]["Time"].get<float>()) + "s";
                highScoreBanner->setDontDraw(false);
                glm::vec3 adjustedPos = getEntityFromName<UiElement>(entityName)->getPosition();
                adjustedPos.x += 12.5f;
                highScoreBanner->setPosition(adjustedPos);
            }
            else if(entityName == "BtS" && !highscores["Catch!"].empty()) 
            {
                highscoreText = "Highscore: " + std::to_string(highscores["Catch!"][0]["Shapes"].get<int>()) + "/225";
                highScoreBanner->setDontDraw(false);
                glm::vec3 adjustedPos = getEntityFromName<UiElement>(entityName)->getPosition();
                adjustedPos.x += 13.0f;
                highScoreBanner->setPosition(adjustedPos);
            }
            else if(entityName == "SunWaveStart" && !highscores["Sunwave"].empty()) {
                highscoreText = "Highscore: " + std::to_string((int)highscores["Sunwave"][0]["Time"].get<float>()) + 
                               "s, " + std::to_string(highscores["Sunwave"][0]["Shapes"].get<int>()) + "/75";
                highScoreBanner->setDontDraw(false);
                glm::vec3 adjustedPos = getEntityFromName<UiElement>(entityName)->getPosition();
                adjustedPos.x -= 2.0f;
                adjustedPos.y -= 2.0f;
                highScoreBanner->setPosition(adjustedPos);
            }
            else {
                highScoreBanner->setDontDraw(true);
                return;
            }

            highScoreBanner->setTextToBeRenderd(highscoreText);
        },
        [this]() -> bool {
            return !inLevelSelector;
        }
    );

}

void GameEnvironment::prepareForLevelChange()
{
    physicsEngine->clearPhysicsObjects();
    entities.clear();
    refColliderForMouseCurrent = nullptr;
    refColliderForMouseOld = nullptr;
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

GameEnvironment::GameEnvironment()
    : window(glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SunWave")),
      ui(window, this),
      physicsEngine(std::make_unique<PhysicsEngine>()),
      minigameManager(std::make_unique<MinigameManager>(this))
{   
    //SoundPrep
    audio.init();
    audio.setGlobalVolume(0.1f);
    loadAllMusic();
}

void GameEnvironment::run()
{
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


    //Load Main Menu
    loadMenu();
 
    glfwMaximizeWindow(window);

    //Loop
    while (!glfwWindowShouldClose(window))
    {   
        //Input
        processInput(window);

        //Rendering
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //"z" Buffer - depth testing

        //Update
        update();
        
        //Draw
        if(ui.getShowGrid())
            grid.drawGrid(ui.getGridSize());
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

void GameEnvironment::playMusicByName(const std::string &musicName)
{
    // Don't restart if same music is playing
    if (currentlyPlayingMusic == musicName && 
        audio.getActiveVoiceCount() > 0 && 
        audio.isValidVoiceHandle(musicHandle)) {
        return;
    }

    // Stop current music
    audio.stopAll();

    // Play new music
    if (musicMap.find(musicName) != musicMap.end()) 
    {
        musicHandle = audio.play(musicMap[musicName]);
        currentlyPlayingMusic = musicName;
    }
}

void GameEnvironment::loadAllMusic()
{
    std::filesystem::path basePath = std::filesystem::current_path();
    fileReader::trimDownPathToWorkingDirectory(basePath);
    basePath /= "assets/audio";

    // Define music files
    std::vector<std::pair<std::string, std::string>> musicFiles = {
        {"menu", "BackgroundMusic.wav"},
        {"shape", "Shape.wav"},
        {"go", "Go.wav"},
        {"catch", "Catch.wav"},
        {"sunwave", "Sunwave.wav"},
        // Add more music files here
    };

    // Load all music files
    for (const auto& [name, filename] : musicFiles) 
    {
        auto path = basePath / filename;
        musicMap[name].load(path.string().c_str());
        musicMap[name].setSingleInstance(true);
        musicMap[name].setLooping(true);
    }
}

