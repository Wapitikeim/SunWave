#pragma once
#define SCREEN_WIDTH 1920.0f
#define SCREEN_LENGTH 1080.0f
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>
//io.hpp overloads the << operator (e.g std::cout)
#include <glm/gtx/io.hpp>

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
#include "entities/Shape.h"
#include "entities/PlayerShape.h"

//Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//ComponentTesting
#include "physics/CollisionTester.h"
#include "physics/PhysicsEngine.h"
#include "components/Component.h"
#include "components/PhysicsCollider.h"

class GameEnvironment 
{
    private:

        //Physics testing
        PhysicsEngine physicsEngine;

        //Framebuffer Testing for Texture->Shader
        GLuint fbo;
        GLuint texture;

        void createFrameBufferAndAttachTexture();

        //Zoom
        float fov = 45.0f;
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        //Camera
        glm::mat4 view;
        //glm::vec3 cameraPos = glm::vec3(30.f, 20.f, 30.f);  
        glm::vec3 cameraPos = glm::vec3(22.1f, 12.426f, 30.f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
        float yHalf = std::sin(glm::radians(fov/2)) * (cameraPos.z/glm::sin(glm::radians((180-fov/2-90))));
        float xHalf = yHalf*SCREEN_WIDTH/SCREEN_LENGTH;
        glm::vec3 whatCameraSeesBottomLeft = glm::vec3(cameraPos.x-xHalf, cameraPos.y-yHalf,0);
        glm::vec3 whatCameraSeesTopLeft = glm::vec3(cameraPos.x-xHalf, cameraPos.y+yHalf,0);;
        glm::vec3 whatCameraSeesTopRight = glm::vec3(cameraPos.x+xHalf, cameraPos.y+yHalf,0);;
        glm::vec3 whatCameraSeesBottomRight = glm::vec3(cameraPos.x+xHalf, cameraPos.y-yHalf,0);;
        

        //DeltaTime
        float deltaTime = 0.0f;	// Time between current frame and last frame
        float lastFrame = 0.0f; // Time of last frame
        double prevTime = glfwGetTime();
        int fps = 0;
        int imGuiFPS = 0;

        //Mouse Movement
        bool firstMouse = true;
        glm::vec3 direction;
        float lastX = SCREEN_WIDTH/2, lastY = SCREEN_LENGTH/2;
        float pitch = 0.0f;
        float yaw = -90.f; 
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);

        //Entities
        std::vector<std::unique_ptr<Entity>> entities;
        void drawEntities();

        

        //Grab Input for ESC key
        void processInput(GLFWwindow* window);      

        //Update Delta
        void updateDeltaTime();

        //Window
        GLFWwindow* window;

        //Update
        void update();

        //Entity Stuff
        void initEntities();
        void resetLevel();
        bool showGrid=true;
        float gridSize = 1.f;

        template<typename entityTypeToGet>
        entityTypeToGet* getEntityFromName(std::string entityName)
        {
            for(auto& entry: entities)
                if(entry->getEntityName() == entityName)
                    return dynamic_cast<entityTypeToGet*>(entry.get());
            return nullptr;
        };

        void deleteEntityFromName(std::string entityName)
        {
            std::cout << "Trying to delete " << entityName << "\n";
            if(getEntityFromName<Entity>(entityName) == nullptr)
            {
                std::cout << entityName << " dosent exist. Cannot remove. \n";
                return;
            }
                
            physicsEngine.unregisterCollider(getComponentOfEntity<PhysicsCollider>(entityName,"Physics"));
            for(auto& entry:entities)
                if(entry->getEntityName() == entityName)
                {
                    entities.erase(std::remove(entities.begin(), entities.end(), entry),entities.end());
                    break;
                }
            std::cout << "Deleted " << entityName << "\n";
                    
        };
        
        template<class componentToGet, typename = std::enable_if<std::is_base_of<Component, componentToGet>::value> >
        componentToGet* getComponentOfEntity(std::string entityName, std::string componentName)
        {
            for(auto& entry: entities)
                if(entry->getEntityName() == entityName)
                    return dynamic_cast<componentToGet*>(entry->getComponent(componentName));
            return nullptr;
        };

        //ImGui 
        void setupImGui();
        void drawImGuiWindows();

    public:
        GameEnvironment();
        ~GameEnvironment()
        {
            glfwTerminate();
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        };

        void run();

};