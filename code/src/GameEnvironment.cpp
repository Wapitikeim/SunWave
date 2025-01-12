#include "GameEnvironment.h"
#include <bitset>

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

void GameEnvironment::mousePositionUpdate()
{
    //MouseCursor Update
    float yPos = glm::abs(ImGui::GetMousePos().y-glfwPrep::getCurrentWindowHeight());
    float xPos = ImGui::GetMousePos().x;
    //float mouseX = (ImGui::GetMousePos().x*(xHalf*2))/glfwPrep::getCurrentWindowWidth();
    mouseX = (xPos*(glm::abs((xHalf*2))-glm::abs(whatCameraSeesBottomLeft.x)))/(float)glfwPrep::getCurrentWindowWidth();
    mouseY = (yPos*(glm::abs((yHalf*2))-glm::abs(whatCameraSeesBottomLeft.y)))/(float)glfwPrep::getCurrentWindowHeight();
    float rot = 0;
    //!!!Erst wenn das Window einmal verändert wird?! -> glfwMaximizeWindow
    auto refCollider = physicsEngine->getFirstColliderShellCollidesWith(glm::vec3(mouseX,mouseY,0),glm::vec3(0.01f),rot);
    if(!pressedAndHoldingSomething)
        refColliderForMouseCurrent = refCollider;
    
    //ChangeShaderByOverOverEffect---
    if(refCollider != nullptr && !pressedAndHoldingSomething)
        refCollider->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",glm::vec4(1));
    //CheckIn/OutBound 
    if(refColliderForMouseCurrent != refColliderForMouseOld)
    {
        if(refColliderForMouseOld != nullptr)
            refColliderForMouseOld->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange",glm::vec4(0,0,0,1));
        
        refColliderForMouseOld = refColliderForMouseCurrent;
    }
    //---ChangeShaderByOverOverEffect
    mouseClickLogic();

    //Rotation Logic
    if(pressedAndHoldingSomething)
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
    }
}

void GameEnvironment::mouseClickLogic()
{
    //0 = Release
    //1 = Press 
    currentMouseLeftButtonState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    if(refColliderForMouseCurrent == nullptr)
        return;
    refColliderForMouseCurrent->updateCornerPositions();
    physicsEngine->addColliderIntoHashTable(refColliderForMouseCurrent);
    if (currentMouseLeftButtonState!= lastMouseLeftButtonState)
    {
        if(!lastMouseLeftButtonState)
        {
            //From Release to Press ("It was Resting now Pressed")
            pressedAndHoldingSomething = true;
            staticPrevRef = refColliderForMouseCurrent->getIsStatic(); 
            refColliderForMouseCurrent->setIsStatic(staticPrevRef);        
        }
        else
        {
            //From Press to Release ("It was Pressed now released")
            pressedAndHoldingSomething = false;
            refColliderForMouseCurrent->setIsStatic(staticPrevRef); 
        }
        lastMouseLeftButtonState = currentMouseLeftButtonState;
    }
    if(pressedAndHoldingSomething)
    {
        if(refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getEntityName() == "ShapeToFindHere" && !shapeFound)
        {
            shapeFound = true;
            physicsEngine->setIsHalting(false);
            deleteEntityFromName("WallBottom");
            registerFunctionToExecuteWhen(5.f,[this]() {this->startMiniGameLogic();});
        }    
        
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
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

void GameEnvironment::initEntities()
{
    //Player? Prep
    auto playerShape = std::make_unique<PlayerShape>("Player", glm::vec3(5.f,5.f,0.0f), glm::vec3(1.f), 0.0f, true, "box");
    entities.push_back(std::move(playerShape));
    entities[0]->addComponent(std::make_unique<PhysicsCollider>(entities[0].get(),0));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics")));

    //Entities Prep
    auto wallBottom = std::make_unique<Shape>("WallBottom", glm::vec3(xHalf,0.5f,0.3f),glm::vec3(xHalf,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallBottom));
    entities[1]->addComponent(std::make_unique<PhysicsCollider>(entities[1].get(),1));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[1]->getComponent("Physics")));

    auto wallTop = std::make_unique<Shape>("WallTop", glm::vec3(xHalf,yHalf*2-0.5f,0.3f),glm::vec3(xHalf,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallTop));
    entities[2]->addComponent(std::make_unique<PhysicsCollider>(entities[2].get(),1));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[2]->getComponent("Physics")));

    auto wallLeft = std::make_unique<Shape>("wallLeft", glm::vec3(0.5f,yHalf,0.3f),glm::vec3(1.0f,yHalf,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallLeft));
    entities[3]->addComponent(std::make_unique<PhysicsCollider>(entities[3].get(),1));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[3]->getComponent("Physics")));

    auto wallRight = std::make_unique<Shape>("wallRight", glm::vec3(xHalf*2-0.5f,yHalf,0.3f),glm::vec3(1.f,yHalf,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallRight));
    entities[4]->addComponent(std::make_unique<PhysicsCollider>(entities[4].get(),1));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[4]->getComponent("Physics")));

    auto wallMiddle = std::make_unique<Shape>("wallMiddle", glm::vec3(21.f,11.f,0.3f),glm::vec3(3.f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallMiddle));
    entities[5]->addComponent(std::make_unique<PhysicsCollider>(entities[5].get(),1));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[5]->getComponent("Physics")));

    auto wallMiddleLeft = std::make_unique<Shape>("wallMiddleLeft", glm::vec3(10.f,11.f,0.3f),glm::vec3(3.f,1.f,1.0f), 45.0f, true, "box");
    entities.push_back(std::move(wallMiddleLeft));
    entities[6]->addComponent(std::make_unique<PhysicsCollider>(entities[6].get(),1));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[6]->getComponent("Physics")));

    auto aDynamicBox = std::make_unique<Shape>("aDynamicBox", glm::vec3(10.f,5.f,0.3f),glm::vec3(1.f), 0, true, "box");
    entities.push_back(std::move(aDynamicBox));
    entities[7]->addComponent(std::make_unique<PhysicsCollider>(getEntityFromName<Entity>("aDynamicBox"),0));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[7]->getComponent("Physics")));
    
    auto aRandomTriggerBox = std::make_unique<Shape>("aRandomTriggerBox", glm::vec3(cameraPos.x,cameraPos.y,0.3f),glm::vec3(0.3f), 0, true, "circle");
    entities.push_back(std::move(aRandomTriggerBox));
    entities[8]->addComponent(std::make_unique<PhysicsCollider>(entities[8].get(),0));
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[8]->getComponent("Physics")));
    
    /* auto physicsTesting_1 = std::make_unique<Shape>("physicsTesting_1", glm::vec3(cameraPos.x+5.f,cameraPos.y+3.f,0.3f),glm::vec3(0.5f), 0, true, "circle");
    entities.push_back(std::move(physicsTesting_1));
    entities[9]->addComponent(std::make_unique<PhysicsCollider>(entities[9].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[9]->getComponent("Physics")));

    auto physicsTesting_2 = std::make_unique<Shape>("physicsTesting_2", glm::vec3(cameraPos.x+8.f,cameraPos.y+6.f,0.3f),glm::vec3(0.75f), 0, true, "circle");
    entities.push_back(std::move(physicsTesting_2));
    entities[10]->addComponent(std::make_unique<PhysicsCollider>(entities[10].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[10]->getComponent("Physics")));

    auto physicsTesting_3 = std::make_unique<Shape>("physicsTesting_3", glm::vec3(cameraPos.x+10.f,cameraPos.y+8.f,0.3f),glm::vec3(1), 0, true, "circle");
    entities.push_back(std::move(physicsTesting_3));
    entities[11]->addComponent(std::make_unique<PhysicsCollider>(entities[11].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[11]->getComponent("Physics")));

    auto physicsTesting_4 = std::make_unique<Shape>("physicsTesting_4", glm::vec3(cameraPos.x-5.f,cameraPos.y+3.f,0.3f),glm::vec3(0.7f), 0, true, "circle");
    entities.push_back(std::move(physicsTesting_4));
    entities[12]->addComponent(std::make_unique<PhysicsCollider>(entities[12].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[12]->getComponent("Physics")));

    auto physicsTesting_5 = std::make_unique<Shape>("physicsTesting_5", glm::vec3(cameraPos.x-8.f,cameraPos.y+5.f,0.3f),glm::vec3(1.2f), 0, true, "circle");
    entities.push_back(std::move(physicsTesting_5));
    entities[13]->addComponent(std::make_unique<PhysicsCollider>(entities[13].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[13]->getComponent("Physics")));

    auto physicsTesting_6 = std::make_unique<Shape>("physicsTesting_6", glm::vec3(cameraPos.x-12.f,cameraPos.y+8.f,0.3f),glm::vec3(2.1f), 0, true, "box");
    entities.push_back(std::move(physicsTesting_6));
    entities[14]->addComponent(std::make_unique<PhysicsCollider>(entities[14].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[14]->getComponent("Physics")));

    auto physicsTesting_7 = std::make_unique<Shape>("physicsTesting_7", glm::vec3(cameraPos.x-16.f,cameraPos.y+6.f,0.3f),glm::vec3(1.3f), 0, true, "cross");
    entities.push_back(std::move(physicsTesting_7));
    entities[15]->addComponent(std::make_unique<PhysicsCollider>(entities[15].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[15]->getComponent("Physics"))); */
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
    initEntities();
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

void GameEnvironment::fillSceneWithEntitys()
{
    prepareForLevelChange();

    physicsEngine->setIsHalting(true);

    //Walls
    {
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
                physicsEngine->registerPhysicsCollider(getComponentOfEntity<PhysicsCollider>(name,"Physics"));
            }
        }
    }
}

void GameEnvironment::updateFunctionEvents()
{
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

void GameEnvironment::startMiniGameLogic()
{
    shapeFound = false;
    fillSceneWithEntitys();
}

void GameEnvironment::registerFunctionToExecuteWhen(float whenFunctionShouldStart, std::function<void()> functionToExecute)
{
    funcExecute t;
    t.timer = whenFunctionShouldStart;
    t.function = functionToExecute;
    functionsToExecuteAfterTime.push_back(t);
    std::cout << "Function Registerd with timer: " << t.timer << "\n";
}

void GameEnvironment::setupImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void GameEnvironment::drawImGuiWindows()
{
    //Needs to be on Top That it dosent crash if other levels are loaded and the Header is collpased
    ImGui::Begin("World Control");
    ImGui::Checkbox("Grid", &showGrid);
    ImGui::SliderFloat("Grid size:", &gridSize, 0.1, 3, "%.3f",0);
    if(ImGui::CollapsingHeader("Entitys Info"))
    {
        for(int i=0; i< entities.size(); i++)
        {
            auto* colliderRef = getComponentOfEntity<PhysicsCollider>(entities[i]->getEntityName(),"Physics");
            ImGui::PushID(i);
            ImGui::Text("%s", entities[i]->getEntityName().c_str());
            if(ImGui::CollapsingHeader("Physics"))
            {
                ImGui::Text("X: %f Y: %f", colliderRef->getPos().x,colliderRef->getPos().y);
                ImGui::Text("Velocity: X: %f Y: %f", colliderRef->getVelocity().x, colliderRef->getVelocity().y);
                ImGui::Text("Elasicity: %f", colliderRef->getElascicity());
                
                bool iTrigger = colliderRef->getIsTrigger();
                ImGui::Checkbox("Is Trigger", &iTrigger);
                if(ImGui::Button("Change Trigger"))
                    colliderRef->setIsTrigger(!iTrigger);
                
                bool iResting = colliderRef->getIsResting();
                ImGui::Checkbox("Is Resting", &iResting);
                
            
                bool iStatic = colliderRef->getIsStatic();
                ImGui::Checkbox("Is Static", &iStatic);
                if(ImGui::Button("Change Static"))
                    colliderRef->setIsStatic(!iStatic);

                bool iDontDraw = entities[i]->getDontDraw();
                ImGui::Checkbox("Dont Draw", &iDontDraw);
                if(ImGui::Button("Hide"))
                    entities[i]->setDontDraw(!iDontDraw);

                if(ImGui::Button("ChangeColor"))
                    entities[i]->getShaderContainer().setUniformVec4("colorChange", glm::vec4(1,0,0,1));
            
                float massRef = colliderRef->getBody().mass;
                ImGui::SliderFloat("Mass:", &massRef, 0.1, 10, "%.3f",0);
                colliderRef->setMass(massRef);

                if(physicsEngine->getHashTableIndicesSize() != 0)
                    ImGui::Text("HashTable Index: %i", colliderRef->getTableIndicies()[0]);
                
            }

            ImGui::PopID();    
            ImGui::Text("");
        }
    }    
    ImGui::End();
    
    ImGui::Begin("Info Panel");
    ImGui::Text("FPS: %i", imGuiFPS);
    ImGui::Text("Entities in Scene: %i", entities.size());
    ImGui::Text("Collisions resolved LastTick: %i", physicsEngine->getCurrentCollisions());
    //ImGui::Text("Ticks calculated last Second: %i", physicsEngine->getTicksLastSecond());
    ImGui::End();

    ImGui::Begin("Physics Engine Control");
    ImGui::SliderFloat("Tickrate:", &physicsEngine->tickrateOfSimulation, 60, 300, "%.3f",0);
    ImGui::SliderFloat("Speed:", &physicsEngine->speedOfSimulation, 0, 3, "%.3f",0);
    bool engineHalting = physicsEngine->getIsHalting();
    ImGui::Checkbox("Is Halting", &engineHalting);
    if(ImGui::Button("Halt Engine"))
        physicsEngine->setIsHalting(!engineHalting);
    if(ImGui::Button("Reset Level"))
        resetLevel();
    if(ImGui::Button("Load just walls"))
        loadWallLevel();
    ImGui::SliderInt("Entity Count", &entitiesToFill, 20, 200);
    if(ImGui::Button("Test Stuff"))
    {
        fillSceneWithEntitys();
    }
    float newBounce = physicsEngine->getBounceMultiplier();
    ImGui::SliderFloat("Bounce multiplier:", &newBounce, 0.5f, 4.0f, "%.3f",0);
    physicsEngine->setBounceMultiplier(newBounce);  
    ImGui::Text("Hash Table Size: %i ", physicsEngine->getHashTableIndicesSize());
    ImGui::End();
     
    ImGui::Begin("Player Extra Info");
        if(getEntityFromName<Entity>("Player") != nullptr)
        {
            ImGui::Text("Player Pos: X: %f Y: %f", getEntityFromName<PlayerShape>("Player")->getPosition().x, getEntityFromName<PlayerShape>("Player")->getPosition().y);
            ImGui::Text("Player is grounded: %s", getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getIsGrounded() ? "true" : "false");
            ImGui::Text("Player Velocity X:%f Y: %f", getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getVelocity().x, getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getVelocity().y);
            ImGui::Text("Player Acceleration X: %f Y:%f",  getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getAcceleration().x, getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getAcceleration().y);
            ImGui::Text("Corner left bottom: X:%f Y:%f", getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftBottom.x, getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftBottom.y);
            ImGui::Text("Corner right bottom: X:%f Y:%f", getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightBottom.x, getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightBottom.y);
            ImGui::Text("Corner left top: X:%f Y:%f", getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftTop.x, getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftTop.y);
            ImGui::Text("Corner right top: X:%f Y:%f", getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightTop.x, getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().rightTop.y);
            //ImGui::Text("TestSDF d=%f",CollisionTester::signedDistance2DBoxAnd2DBox(getComponentOfEntity<PhysicsCollider>("aRandomTriggerBox","Physics"),getComponentOfEntity<PhysicsCollider>("Player","Physics")));
            ImGui::SliderFloat("Speed", &getEntityFromName<PlayerShape>("Player")->velocity,1, 20,"%.3f",0);
            float scaleX = getEntityFromName<Entity>("Player")->getScale().x;
            float scaleY = getEntityFromName<Entity>("Player")->getScale().y;
            ImGui::SliderFloat("ScaleX:", &scaleX, 0.1, 30, "%.3f",0);
            ImGui::SliderFloat("ScaleY:", &scaleY, 0.1, 30, "%.3f",0);
            glm::vec3 newScale(scaleX,scaleY, getEntityFromName<Entity>("Player")->getScale().z);
            getEntityFromName<Entity>("Player")->setScale(newScale);
            ImGui::Text("Player rot %f", getEntityFromName<Entity>("Player")->getRotation());
            
        }
        else
            ImGui::Text("Player not found");
    ImGui::End();
    
    ImGui::Begin("Mouse Information");
    ImGui::Text("Mouse X: %f", mouseX);
    ImGui::Text("Mouse Y: %f", mouseY);
    if(refColliderForMouseCurrent == nullptr)
        ImGui::Text("Mouse in contact with: none");
    else
        ImGui::Text("Mouse in contact with: %s", refColliderForMouseCurrent->getEntityThisIsAttachedTo()->getEntityName().c_str());
    ImGui::End();

    //Nessecary
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GameEnvironment::GameEnvironment()
{
    window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hidden Instinct");
}

//Framebuffer Testing
void GameEnvironment::createFrameBufferAndAttachTexture()
{
    //Create + Bind
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1,&texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920 , 1080, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

}

//Maybe handeld through IMGUI -> Discard?
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
    initEntities();
    //ImGui
    setupImGui();
 
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

        //Imgui Setup
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        

        //Physics pre Update
        physicsEngine->getInitialTransform(deltaTime);

        //FPS
        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - prevTime >= 1.)
        {
            imGuiFPS = fps;
            fps = 0;
            prevTime = currentTime;
        }
        
        //Camera Update (Theoriactically)
        Camera::setCurrentCameraView(glm::lookAt(cameraPos, cameraPos + cameraFront, up));
        Camera::setCurrentCameraProjection(glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f));

        mousePositionUpdate();
        //GameLogic Testing        
        updateFunctionEvents();

        //Grid First for transperent Textures ->They get cut off if they enter the -y plane though 
        update();
        if(showGrid)
            grid.drawGrid(gridSize);
        //"Physics" Reset AFTER Update
        physicsEngine->updatePhysics(); 
          
        drawEntities();
        drawImGuiWindows();
        glfwSwapBuffers(window);
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }

    
}

void GameEnvironment::testing(const glm::vec3& ePos)
{
    // Define an offset to handle negative coordinates (assuming a range large enough for your application)
    constexpr int32_t OFFSET = 32768;
    constexpr float BUCKETSIZE = 1.f;
    //constexpr int32_t OFFSET = 100; -6653/2 + 6653 In der X achse

    // Helper function to interleave bits of a 16-bit integer
    auto interleaveBits = [](uint16_t n)
    {
        uint32_t x = n;
        x = (x | (x << 8)) & 0x00FF00FF;
        x = (x | (x << 4)) & 0x0F0F0F0F;
        x = (x | (x << 2)) & 0x33333333;
        x = (x | (x << 1)) & 0x55555555;
        return x;
    };
    // Morton encoding function for integer bucket coordinates
    auto mortonEncode2D = [OFFSET, interleaveBits](int bucketX, int bucketY) 
    {
        // Shift negative coordinates to non-negative space
        uint16_t xBits = static_cast<uint16_t>(bucketX + OFFSET);
        uint16_t yBits = static_cast<uint16_t>(bucketY + OFFSET);

        // Interleave bits to create Morton code
        return interleaveBits(xBits) | (interleaveBits(yBits) << 1);
    };
    
    int bucketX = glm::floor(ePos.x/BUCKETSIZE);
    int bucketY = glm::floor(ePos.y/BUCKETSIZE);
    std::cout <<"Current Bucket: " << std::bitset<32>(mortonEncode2D(bucketX,bucketY)) << "\n";
    
}
