#include "GameEnvironment.h"

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

void GameEnvironment::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

float GameEnvironment::signedDistance2DBox(glm::vec3 posToCheckTo, glm::vec3 objectScale, glm::vec3 objectPos, float rotation)
{
    float angle = rotation *3.14159 * 2 * -1;
    objectPos.x = (objectPos.x * glm::cos(angle)) - (objectPos.y * glm::sin(angle));
    objectPos.y = (objectPos.x * glm::sin(angle)) + (objectPos.y * glm::cos(angle));
    //Translation
    glm::vec2 position = glm::vec2(posToCheckTo.x, posToCheckTo.y)-glm::vec2(objectPos.x, objectPos.y);
    
    glm::vec2 distance = glm::abs(position) - glm::vec2(objectScale.x,objectScale.y);
    
    //glm::vec2 distance = glm::abs(posToCheckTo) - (objectScale);
    //std::cout << distance.x << " " << distance.y << "\n";
    return glm::length(glm::max(distance,0.0f)) + glm::min(glm::max(distance.x, distance.y),0.0f);
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
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics")));

    //Entities Prep
    auto wallBottom = std::make_unique<Shape>("WallBottom", glm::vec3(21.1f,0.0f,0.3f),glm::vec3(22.f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallBottom));
    entities[1]->addComponent(std::make_unique<PhysicsCollider>(entities[1].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[1]->getComponent("Physics")));

    auto wallTop = std::make_unique<Shape>("WallTop", glm::vec3(21.1f,22.f,0.3f),glm::vec3(22.f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallTop));
    entities[2]->addComponent(std::make_unique<PhysicsCollider>(entities[2].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[2]->getComponent("Physics")));

    auto wallLeft = std::make_unique<Shape>("wallLeft", glm::vec3(-1.0f,11.f,0.3f),glm::vec3(1.0f,11.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallLeft));
    entities[3]->addComponent(std::make_unique<PhysicsCollider>(entities[3].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[3]->getComponent("Physics")));

    auto wallRight = std::make_unique<Shape>("wallRight", glm::vec3(42.8f,11.f,0.3f),glm::vec3(1.f,11.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallRight));
    entities[4]->addComponent(std::make_unique<PhysicsCollider>(entities[4].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[4]->getComponent("Physics")));

    auto wallMiddle = std::make_unique<Shape>("wallMiddle", glm::vec3(21.f,11.f,0.3f),glm::vec3(3.f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallMiddle));
    entities[5]->addComponent(std::make_unique<PhysicsCollider>(entities[5].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[5]->getComponent("Physics")));

    auto wallMiddleRight = std::make_unique<Shape>("wallMiddle", glm::vec3(10.f,11.f,0.3f),glm::vec3(3.f,1.f,1.0f), 45.0f, true, "box");
    entities.push_back(std::move(wallMiddleRight));
    entities[6]->addComponent(std::make_unique<PhysicsCollider>(entities[6].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[6]->getComponent("Physics")));

    auto aDynamicBox = std::make_unique<Shape>("aDynamicBox", glm::vec3(10.f,5.f,0.3f),glm::vec3(1.f), 0, true, "box");
    entities.push_back(std::move(aDynamicBox));
    entities[7]->addComponent(std::make_unique<PhysicsCollider>(entities[7].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[7]->getComponent("Physics")));
    
    auto aRandomTriggerBox = std::make_unique<Shape>("aRandomTriggerBox", glm::vec3(21.15f,13.f,0),glm::vec3(1.f,2.f,1.f), 45, true, "box");
    entities.push_back(std::move(aRandomTriggerBox));
    entities[8]->addComponent(std::make_unique<PhysicsCollider>(entities[8].get(),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[8]->getComponent("Physics")));
    dynamic_cast<PhysicsCollider*>(entities[8]->getComponent("Physics"))->setIsTrigger(1);
}

void GameEnvironment::resetLevel()
{
    entities.clear();
    physicsEngine.clearPhysicsObjects();
    initEntities();
}

GameEnvironment::GameEnvironment()
{
    window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_LENGTH, "Game");
}


//Physics Testing
void GameEnvironment::activateKinematic(Entity* e)
{
    _kinematicActivated = true;

    _initialVelocity = glm::vec2(cos(_angle*3.14159f/180.f), sin(_angle*3.14159f /180.f)) *_power;
    _initialPosition = glm::vec2(e->getPosition().x, e->getPosition().y);
}

void GameEnvironment::updateKinematics(Entity* e)
{
    _time += deltaTime;

    float newPosX = kinematicCalculation(0,_initialVelocity.x, _initialPosition.x, _time);
    float newPosY = kinematicCalculation(-0.981f, _initialVelocity.y, _initialPosition.y, _time);

    e->setPosition(glm::vec3(newPosX,newPosY,e->getPosition().z));

}

float GameEnvironment::kinematicCalculation(float acceleration, float velocity, float position, float time)
{
    return 0.5f*acceleration*time*time + velocity*time + position;
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

void GameEnvironment::run()
{
    initEntities();
    //Camera Prep
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up); 
    Camera::setCurrentCameraView(view);//Prep if no Camera Flight active
    
    //Mouse
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    //glfwSetCursorPosCallback(window, mouse_callback);
    
    //Zoom
    //glfwSetScrollCallback(window, scroll_callback); 

    //Grid Feature
    InfiniteGrid grid(ShaderContainer("gridVertexShader.vert", "gridFragmentShader.frag"));

    //IMGUI ###############
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
    //#################

    //Loop
    while (!glfwWindowShouldClose(window))
    {   
        //Input
        processInput(window);
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
        physicsEngine.getInitialTransform(deltaTime);

        //FPS
        double currentTime = glfwGetTime();
        fps++;
        if (currentTime - prevTime >= 1.)
        {
            imGuiFPS = fps;
            fps = 0;
            prevTime = currentTime;
        }
        
        srand(time(0));
        dynamic_cast<PhysicsCollider*>(entities[7]->getComponent("Physics"))->applyForce(glm::vec3(rand()%50+(-25),rand()%50+(-25),0));

        //std::cout << glm::distance(entities[0]->getPosition(), entities[7]->getPosition()) << "\n";
        //Kinematic testing stuff
        /*
        if(!_kinematicActivated)
        {
            activateKinematic(entities[0].get());
            std::cout << "Init Pos: X: " << _initialPosition.x << " Y: " << _initialPosition.y << "\n";
            std::cout << "Init Velo: X: " << _initialVelocity.x << " Y: " << _initialVelocity.y << "\n";
        }
        else
            updateKinematics(entities[0].get());
        std::cout << entities[0]->getPosition().x << " " << entities[0]->getPosition().y << "\n";
        */
        //Camera Update (Theoriactically)
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up); 
        Camera::setCurrentCameraView(view);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_LENGTH, 0.1f, 100.0f);
        Camera::setCurrentCameraProjection(projection);

        //Grid First for transperent Textures ->They get cut off if they enter the -y plane though 
        update();
        grid.drawGrid(1.f);
        
        //"Physics" Reset AFTER Update
        physicsEngine.updatePhysics();
        //#################    
        
        drawEntities();

        //Imgui rendering ###############
        ImGui::Begin("Info Panel");
        ImGui::Text("FPS: %i", imGuiFPS);
        ImGui::Text("Player Pos: X: %f Y: %f", entities[0]->getPosition().x, entities[0]->getPosition().y);
        ImGui::Text("Entities in Scene: %i", entities.size());
        ImGui::Text("Player is grounded: %s", dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getIsGrounded() ? "true" : "false");
        ImGui::Text("Player Velocity X:%f Y: %f", dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getVelocity().x, dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getVelocity().y);
        ImGui::Text("Player Acceleration X: %f Y:%f",  dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getAcceleration().x, dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getAcceleration().y);
        ImGui::SliderFloat("Speed", &dynamic_cast<PlayerShape*>(entities[0].get())->velocity,10, 25,"%.3f",0);
        ImGui::End();

        ImGui::Begin("Physics Engine Control");
        ImGui::SliderFloat("Tickrate:", &physicsEngine.tickrateOfSimulation, 60, 300, "%.3f",0);
        ImGui::SliderFloat("Speed:", &physicsEngine.speedOfSimulation, 0.1, 3, "%.3f",0);
        if(ImGui::Button("Reset Level"))
            resetLevel();
        ImGui::End();

        ImGui::Begin("Player Extra Info");
        ImGui::Text("Corner left bottom: X:%f Y:%f", dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getCornerPos().leftBottom.x, dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))->getCornerPos().leftBottom.y);
        ImGui::Text("TestSDF d=%f",CollisionTester::signedDistance2DBoxAnd2DBox(dynamic_cast<PhysicsCollider*>(entities[8]->getComponent("Physics")),dynamic_cast<PhysicsCollider*>(entities[0]->getComponent("Physics"))));
        ImGui::End();

        

        // Render dear imgui into screen
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //############

        glfwSwapBuffers(window);
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }

    
}
