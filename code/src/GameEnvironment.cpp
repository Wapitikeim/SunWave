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
    /* const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
        Camera::setCurrentCameraView(view);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
        Camera::setCurrentCameraView(view);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
        Camera::setCurrentCameraView(view);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, up);
        Camera::setCurrentCameraView(view); */
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
    //Player? Prep
    auto playerShape = std::make_unique<PlayerShape>("Player", glm::vec3(5.f,5.f,0.0f), glm::vec3(1.23f,1.45f,1.0f), 0.0f, true, "box");
    entities.push_back(std::move(playerShape));

    //Entities Prep
    auto simpleBox = std::make_unique<Shape>("SimpleBox", glm::vec3(1.0f,10.0f,0.3f),glm::vec3(1.f,1.f,1.0f), 0.0f, true, "cross");
    entities.push_back(std::move(simpleBox));
    auto simpleBox2 = std::make_unique<Shape>("SimpleBox2", glm::vec3(0,0,0.3f),glm::vec3(1.f,1.f,1.0f), 0.0f, true, "circle");
    entities.push_back(std::move(simpleBox2));

    //Camera Prep
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up); 
    Camera::setCurrentCameraView(view);//Prep if no Camera Flight active
    
    //Mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    //glfwSetCursorPosCallback(window, mouse_callback);
    
    //Zoom
    //glfwSetScrollCallback(window, scroll_callback); 

    //Grid Feature
    InfiniteGrid grid(ShaderContainer("gridVertexShader.vert", "gridFragmentShader.frag"));

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
        
        //Kinematic testing stuff
        /*
        if(!_kinematicActivated)
        {
            activateKinematic(entities[1].get());
            std::cout << "Init Pos: X: " << _initialPosition.x << " Y: " << _initialPosition.y << "\n";
            std::cout << "Init Velo: X: " << _initialVelocity.x << " Y: " << _initialVelocity.y << "\n";
        }
        else
            updateKinematics(entities[1].get());
        std::cout << entities[1]->getPosition().x << " " << entities[1]->getPosition().y << "\n";
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
        drawEntities();
        glfwSwapBuffers(window);
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }

    
}
