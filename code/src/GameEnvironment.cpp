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
    auto wallBottom = std::make_unique<Shape>("WallBottom", glm::vec3(22.08f,0.5f,0.3f),glm::vec3(22.08f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallBottom));
    entities[1]->addComponent(std::make_unique<PhysicsCollider>(entities[1].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[1]->getComponent("Physics")));

    auto wallTop = std::make_unique<Shape>("WallTop", glm::vec3(22.08f,24.34f,0.3f),glm::vec3(22.08f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallTop));
    entities[2]->addComponent(std::make_unique<PhysicsCollider>(entities[2].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[2]->getComponent("Physics")));

    auto wallLeft = std::make_unique<Shape>("wallLeft", glm::vec3(0.5f,12.426f,0.3f),glm::vec3(1.0f,12.426f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallLeft));
    entities[3]->addComponent(std::make_unique<PhysicsCollider>(entities[3].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[3]->getComponent("Physics")));

    auto wallRight = std::make_unique<Shape>("wallRight", glm::vec3(44.1f,12.426f,0.3f),glm::vec3(1.f,12.426f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallRight));
    entities[4]->addComponent(std::make_unique<PhysicsCollider>(entities[4].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[4]->getComponent("Physics")));

    auto wallMiddle = std::make_unique<Shape>("wallMiddle", glm::vec3(21.f,11.f,0.3f),glm::vec3(3.f,1.f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(wallMiddle));
    entities[5]->addComponent(std::make_unique<PhysicsCollider>(entities[5].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[5]->getComponent("Physics")));

    auto wallMiddleLeft = std::make_unique<Shape>("wallMiddleLeft", glm::vec3(10.f,11.f,0.3f),glm::vec3(3.f,1.f,1.0f), 45.0f, true, "box");
    entities.push_back(std::move(wallMiddleLeft));
    entities[6]->addComponent(std::make_unique<PhysicsCollider>(entities[6].get(),1));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[6]->getComponent("Physics")));

    auto aDynamicBox = std::make_unique<Shape>("aDynamicBox", glm::vec3(10.f,5.f,0.3f),glm::vec3(1.f), 0, true, "box");
    entities.push_back(std::move(aDynamicBox));
    entities[7]->addComponent(std::make_unique<PhysicsCollider>(getEntityFromName<Entity>("aDynamicBox"),0));
    physicsEngine.registerPhysicsCollider(dynamic_cast<PhysicsCollider*>(entities[7]->getComponent("Physics")));
    
    auto aRandomTriggerBox = std::make_unique<Shape>("aRandomTriggerBox", glm::vec3(22.1f, 12.426f,0.f),glm::vec3(0.5f), 0, true, "circle");
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
    ImGui::Begin("Info Panel");
    ImGui::Text("FPS: %i", imGuiFPS);
    ImGui::Text("Player Pos: X: %f Y: %f", getEntityFromName<PlayerShape>("Player")->getPosition().x, getEntityFromName<PlayerShape>("Player")->getPosition().y);
    ImGui::Text("Entities in Scene: %i", entities.size());
    ImGui::Text("Player is grounded: %s", getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getIsGrounded() ? "true" : "false");
    ImGui::Text("Player Velocity X:%f Y: %f", getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getVelocity().x, getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getVelocity().y);
    ImGui::Text("Player Acceleration X: %f Y:%f",  getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getAcceleration().x, getComponentOfEntity<PhysicsCollider>("Player", "Physics")->getAcceleration().y);
    ImGui::SliderFloat("Speed", &getEntityFromName<PlayerShape>("Player")->velocity,10, 25,"%.3f",0);
    ImGui::End();

    ImGui::Begin("Physics Engine Control");
    ImGui::SliderFloat("Tickrate:", &physicsEngine.tickrateOfSimulation, 60, 300, "%.3f",0);
    ImGui::SliderFloat("Speed:", &physicsEngine.speedOfSimulation, 0, 3, "%.3f",0);
    if(ImGui::Button("Reset Level"))
        resetLevel();
    ImGui::End();

    ImGui::Begin("Player Extra Info");
    ImGui::Text("Corner left bottom: X:%f Y:%f", getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftBottom.x, getComponentOfEntity<PhysicsCollider>("Player","Physics")->getCornerPos().leftBottom.y);
    ImGui::Text("TestSDF d=%f",CollisionTester::signedDistance2DBoxAnd2DBox(getComponentOfEntity<PhysicsCollider>("aRandomTriggerBox","Physics"),getComponentOfEntity<PhysicsCollider>("Player","Physics")));
    ImGui::End();
    
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
                
                bool iTrigger = colliderRef->getIsTrigger();
                ImGui::Checkbox("Is Trigger", &iTrigger);
                if(ImGui::Button("Change Trigger"))
                    colliderRef->setIsTrigger(!iTrigger);
            
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
                
            }



            ImGui::PopID();    
            ImGui::Text("");
        }
    }    
    
    ImGui::End();

    //Nessecary
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

GameEnvironment::GameEnvironment()
{
    window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_LENGTH, "Game");
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

    //ImGui
    setupImGui();

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
        //Random
        srand(time(0));
        dynamic_cast<PhysicsCollider*>(entities[7]->getComponent("Physics"))->applyForce(glm::vec3(rand()%50+(-25),rand()%50+(-25),0));

        //Camera Update (Theoriactically)
        Camera::setCurrentCameraView(glm::lookAt(cameraPos, cameraPos + cameraFront, up));
        Camera::setCurrentCameraProjection(glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_LENGTH, 0.1f, 100.0f));
        
        //Grid First for transperent Textures ->They get cut off if they enter the -y plane though 
        update();
        if(showGrid)
            grid.drawGrid(gridSize);
        //"Physics" Reset AFTER Update
        physicsEngine.updatePhysics();   
        drawEntities();
        drawImGuiWindows();
        glfwSwapBuffers(window);
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }

    
}
