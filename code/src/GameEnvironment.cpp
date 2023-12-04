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
    const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
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
        Camera::setCurrentCameraView(view);
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

GameEnvironment::GameEnvironment()
{
    window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_LENGTH, "Game");
}

void GameEnvironment::run()
{
    //Entities Prep
    auto heart = std::make_unique<Shape>("Heart", glm::vec3(0.1f,0.1f,0.0f), glm::vec3(0.1f,0.1f,1.0f), 45.0f, "BlockyHeart.png");
    entities.push_back(std::move(heart));
    auto circle = std::make_unique<Shape>("Circle", glm::vec3(0.1f,0.3f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 0.0f, "Circle.png");
    entities.push_back(std::move(circle));
    auto xShape = std::make_unique<Shape>("XShape", glm::vec3(0.1f,0.5f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 0.0f, "XShape.png");
    entities.push_back(std::move(xShape));
    auto simpleBox = std::make_unique<Shape>("SimpleBox", glm::vec3(0.5f,0.7f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 0.0f, "SimpleBox.png");
    entities.push_back(std::move(simpleBox));

    //Camera Prep
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up); 
    Camera::setCurrentCameraView(view);//Prep if no Camera Flight active
    
    //Mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    //glfwSetCursorPosCallback(window, mouse_callback);
    
    //Zoom
    //glfwSetScrollCallback(window, scroll_callback); 

    //Grid Feature
    InfiniteGrid grid(ShaderContainer("gridVertexShader.glsl", "gridFragmentShader.glsl"));

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
        
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), SCREEN_WIDTH / SCREEN_LENGTH, 0.1f, 100.0f);
        Camera::setCurrentCameraProjection(projection);
        
        /* if(signedDistance2DBox(cameraPos, simpleBox.getScale(), simpleBox.getPosition(), simpleBox.getRotation()) <=0)
            simpleBox.setPosition(glm::vec3(simpleBox.getPosition().x, simpleBox.getPosition().y,simpleBox.getPosition().z-0.01f));
        else if(simpleBox.getPosition().z < 0)
            simpleBox.setPosition(glm::vec3(simpleBox.getPosition().x, simpleBox.getPosition().y,simpleBox.getPosition().z+0.0001f));
        xShape.setZRotation(sin(glfwGetTime())*100);
        entities[0]->setZRotation(sin(glfwGetTime())*100);
        simpleBox.setPosition(glm::vec3(simpleBox.getPosition().x,glm::abs(cos(glfwGetTime())),simpleBox.getPosition().z));
        simpleBox.setZRotation(sin(glfwGetTime())*100); */
        
        //Grid First for transperent Textures ->They get cut off if they enter the -y plane though 
        grid.drawGrid(5);
        drawEntities();
        glfwSwapBuffers(window);
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }

    
}
