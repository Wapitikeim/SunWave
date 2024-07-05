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

        //Camera
        glm::mat4 view;
        glm::vec3 cameraPos = glm::vec3(21.f, 10.f, 30.f);  
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

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

        //Zoom
        float fov = 45.0f;
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

        //Grab Input for ESC key
        void processInput(GLFWwindow* window);
        
        //SDF
        float signedDistance2DBox(glm::vec3 posToCheckTo, glm::vec3 objectScale, glm::vec3 objectPos, float rotation);

        //Update Delta
        void updateDeltaTime();

        //Window
        GLFWwindow* window;

        //Update
        void update();

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

        template<typename componentToGet, typename = std::enable_if<std::is_base_of<Component, componentToGet>::value> >
        componentToGet* getComponentOfEntity(std::string entityName, std::string componentName)
        {
            for(auto& entry: entities)
                if(entry->getEntityName() == entityName)
                    return dynamic_cast<componentToGet*>(entry->getComponent(componentName));
            return nullptr;
        };


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