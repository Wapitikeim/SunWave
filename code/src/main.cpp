#define SCREEN_WIDTH 1920.0f
#define SCREEN_LENGTH 1080.0f
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>

#include <iostream>

#include <vector>
#include <memory>
#include <filesystem>

#include <math.h>


#include "util/glfwPrep.h"
#include "util/InfiniteGrid.h"
#include "util/Camera.h"
#include "shaders/ShaderContainer.h"
#include "mesh/MeshContainer.h"
#include "entities/Entity.h"

//Camera
glm::mat4 view;
glm::vec3 cameraPos = glm::vec3(2.2f, 1.05f, 3.00f);  
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

//Mouse Movement
bool firstMouse = true;
glm::vec3 direction;
float lastX = SCREEN_WIDTH/2, lastY = SCREEN_LENGTH/2;
float pitch = 0.0f;
float yaw = -90.f;   
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

//Zoom
float fov = 45.0f;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 90.0f)
        fov = 90.0f; 
}

//Grab Input for ESC key
void processInput(GLFWwindow* window)
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

float signedDistance2DBox(glm::vec3 posToCheckTo, glm::vec3 objectScale, glm::vec3 objectPos, float rotation)
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

void updateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

int main()
{
    //Actually also handles the opengl Draw stuff resize for Window
    GLFWwindow* window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(SCREEN_WIDTH, SCREEN_LENGTH, "Game");

    //MeshStuff
    std::shared_ptr<std::vector<float>> quadrat = std::make_shared<std::vector<float>>();
    quadrat->assign(
    {
    // positions          // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
    }
    );
    std::shared_ptr<std::vector<int>> EBOIndices = std::make_shared<std::vector<int>>();
    EBOIndices->assign({  // note that we start from 0!
    0, 1, 2,   // first triangle
    3, 2, 0    // second triangle
    });

    Entity heart("Heart", ShaderContainer("defaultVertexShader.glsl", "defaultFragmentShader.glsl"), MeshContainer(quadrat, EBOIndices, "BlockyHeart.png"), glm::vec3(0.1f,0.1f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 45.0f);
    Entity circle("Circle", ShaderContainer("defaultVertexShader.glsl", "defaultFragmentShader.glsl"), MeshContainer(quadrat, EBOIndices, "Circle.png"), glm::vec3(0.1f,0.3f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 0.0f);
    Entity xShape("XShape", ShaderContainer("defaultVertexShader.glsl", "defaultFragmentShader.glsl"), MeshContainer(quadrat, EBOIndices, "XShape.png"), glm::vec3(0.1f,0.5f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 0.0f);
    Entity simpleBox("SimpleBox", ShaderContainer("defaultVertexShader.glsl", "defaultFragmentShader.glsl"), MeshContainer(quadrat, EBOIndices, "SimpleBox.png"), glm::vec3(0.5f,0.7f,0.0f),glm::vec3(0.1f,0.1f,1.0f), 0.0f);
    
    

    //Camera Prep
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, up); 
    Camera::setCurrentCameraView(view);//Prep if no Camera Flight active
    
    //Mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 
    glfwSetCursorPosCallback(window, mouse_callback);
    
    //Zoom
    glfwSetScrollCallback(window, scroll_callback); 

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
        
        if(signedDistance2DBox(cameraPos, simpleBox.getScale(), simpleBox.getPosition(), simpleBox.getRotation()) <=0)
            simpleBox.setPosition(glm::vec3(simpleBox.getPosition().x, simpleBox.getPosition().y,simpleBox.getPosition().z-0.01f));
        else if(simpleBox.getPosition().z < 0)
            simpleBox.setPosition(glm::vec3(simpleBox.getPosition().x, simpleBox.getPosition().y,simpleBox.getPosition().z+0.0001f));
        xShape.setZRotation(sin(glfwGetTime())*100);
        heart.setZRotation(sin(glfwGetTime())*100);
        simpleBox.setPosition(glm::vec3(simpleBox.getPosition().x,glm::abs(cos(glfwGetTime())),simpleBox.getPosition().z));
        simpleBox.setZRotation(sin(glfwGetTime())*100);
        //Grid First for transperent Textures ->They get cut off if they enter the -y plane though 
        grid.drawGrid(5);

        heart.draw();
        circle.draw();
        xShape.draw();
        simpleBox.draw();
        glfwSwapBuffers(window);
        
        updateDeltaTime(); 
        //BuffersAndEventHandeling
        glfwPollEvents();
    }


    glfwTerminate();
    return 0;

}