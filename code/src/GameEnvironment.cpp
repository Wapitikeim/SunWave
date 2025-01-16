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
    
    auto testElement = std::make_unique<UiElement>("TestUI",glm::vec3(15,10,0), glm::vec3(1.f), 0, "Ja leck mich fett", "Open_Sans\\static\\OpenSans-Regular.ttf", 48);
    entities.push_back(std::move(testElement));
    entities[9]->addComponent(std::make_unique<PhysicsCollider>(entities[9].get(),0));
    //dynamic_cast<PhysicsCollider*>(entities[9]->getComponent("Physics"))->setIsTrigger(true);
    physicsEngine->registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[9]->getComponent("Physics")));

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

GameEnvironment::GameEnvironment()
    :window(glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hidden Instinct")),
    ui(window,this)
{   
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
        Camera::setCurrentCameraOrto(glm::ortho(0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT));
        mousePositionUpdate();
        //GameLogic Testing        
        updateFunctionEvents();

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

void GameEnvironment::testing(const std::string& text, float x, float y, float scale, glm::vec3 color)
{
    
}
