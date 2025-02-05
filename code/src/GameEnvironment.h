#pragma once
#define SCREEN_WIDTH 1920.0f
#define SCREEN_HEIGHT 1080.0f

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/vec4.hpp>
//io.hpp overloads the << operator (e.g std::cout)
#include <glm/gtx/io.hpp>

#include <iostream>
#include <thread>
#include <future>

#include <vector>
#include <memory>
#include <filesystem>

#include <math.h>
#include <random>

//Util
#include "util/glfwPrep.h"
#include "util/InfiniteGrid.h"
#include "util/Camera.h"

//Shaders and Meshes
#include "shaders/ShaderContainer.h"
#include "mesh/MeshContainer.h"

//Scenes
#include "scenes/SceneManager.h"

//Minigames
#include "minigames/MinigameManager.h"

//Entities
#include "entities/Entity.h"
#include "entities/Shape.h"
#include "entities/PlayerShape.h"
#include "entities/UiElement.h"

//Entity Components
#include "components/Component.h"
#include "components/PhysicsCollider.h"

//UI
#include "ui/UiManager.h"
#include "ui/FontLoader.h"

//Physics
#include "physics/CollisionTester.h"
#include "physics/PhysicsEngine.h"

//Sound
#include <soloud.h>
#include <soloud_wav.h>

class GameEnvironment 
{
    private:

        //Physics engine
        std::unique_ptr<PhysicsEngine> physicsEngine;

        //Camera
        glm::mat4 view; 
        glm::vec3 cameraPos = glm::vec3(22.1f, 12.426f, 30.f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
        glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

        //Zoom
        float fov = 45.0f;
        void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
        
        //World Translate Information
        void updateWorldTranslationInfo();
        float yHalf;
        float xHalf;
        glm::vec3 whatCameraSeesBottomLeft;
        glm::vec3 whatCameraSeesTopLeft;
        glm::vec3 whatCameraSeesTopRight;
        glm::vec3 whatCameraSeesBottomRight;   

        //DeltaTime
        float deltaTime = 0.0f;	// Time between current frame and last frame
        float lastFrame = 0.0f; // Time of last frame
        double prevTime = glfwGetTime();
        int fps = 0;

        //Mouse Movement
        bool firstMouse = true;
        glm::vec3 direction;
        float lastX = SCREEN_WIDTH/2, lastY = SCREEN_HEIGHT/2;
        float pitch = 0.0f;
        float yaw = -90.f; 
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        
        //Mouse
        float mouseX;
        float mouseY;
        bool currentMouseLeftButtonState;
        bool mouseLeftClickHoldingDown = false;
        bool lastMouseLeftButtonState = false;
        bool pressedAndHoldingSomething = false;
        bool staticPrevRef = false;
        bool entityManipulationThroughMouse = true;
        bool hoverOverEffect = true;
        glm::vec4 hoverOverColor = glm::vec4(1);
        glm::vec4 colorOfRefEntity;
        Entity* entityColorSaved;
        PhysicsCollider* refColliderForMouseCurrent = nullptr;
        PhysicsCollider* refColliderForMouseOld = nullptr;
        void mouseUpdate();
        void mouseCursorPositionUpdate();
        void mousePhysicsUpdate();
        void mouseHoverOverEffect();
        void mouseClickUpdate();
        void mouseButtonClickInteractionLogic();
        void mouseEntityManipulationLogic();
        void mouseClickLogic();

        //Level loading
        SceneManager sceneManager;
        
        bool gamePaused = false;
        bool inLevelSelector = false;
        bool inMenu = false;
        bool physicsEngineWasActive = false;
        
        //Entities
        std::vector<std::unique_ptr<Entity>> entities;
        void drawEntities();

        //Global Input Managment
        void processInput(GLFWwindow* window);      

        //Update Delta
        void updateDeltaTime();

        //Window
        GLFWwindow* window;

        //Update
        void update();

        //Ui
        UiManager ui;

        //Game logic Update
        void updateFunctionEvents();
        void updateLoopingFunctions();
        void updateRepeatingFunctions();
        struct funcExecute
        {
            float timer;
            std::function<void()> function;
        };
        struct RepeatingFunction
        {
            std::function<void()> function;
            std::function<bool()> stopCondition;
        };
        struct LoopingFunction 
        {
            float timer;
            std::function<void()> function;
        };
        std::vector<LoopingFunction> loopingFunctions;
        std::vector<funcExecute> functionsToExecuteAfterTime;
        std::vector<RepeatingFunction> repeatingFunctions;
        
        //MiniGames
        std::unique_ptr<MinigameManager> minigameManager;

        //Sound
        SoLoud::Soloud audio;
        SoLoud::Wav menuMusic;
        std::unordered_map<std::string, SoLoud::Wav> musicMap;
        unsigned int musicHandle;
        std::string currentlyPlayingMusic;
        
    public:
        GameEnvironment();
        ~GameEnvironment()
        {
            glfwTerminate();
            audio.deinit();
        };
        
        //Main Loop
        void run();

        //Entity Management
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
                
            physicsEngine->unregisterCollider(getComponentOfEntity<PhysicsCollider>(entityName,"Physics"));
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

        void addEntity(std::unique_ptr<Entity> entityToAdd)
        {
            for(auto &entry:entities)
                if(entry->getEntityName() == entityToAdd->getEntityName())
                {
                    std::cout << "The Name " << entityToAdd->getEntityName() << " already exist in entities.\n";
                    return;
                }
            entities.push_back(std::move(entityToAdd));
        }
        
        std::vector<std::unique_ptr<Entity>>& getEntities() {return entities;}
        
        //Getter
        GLFWwindow* getCurrentWindow(){return window;};
        PhysicsEngine* getPhysicsEngine()const{return physicsEngine.get();};
        SceneManager& getSceneManager() { return sceneManager; }
        const float& getFov()const{return fov;};
        const float& getDeltaTime()const{return deltaTime;};
        const glm::vec3 getCameraPos()const{return cameraPos;};
        const float& getXHalf()const{return xHalf;};
        const float& getYHalf()const{return yHalf;};
        const bool& getIfPhysicsEngineWasActive(){return physicsEngineWasActive;};
        const bool& getIfPressedAndHolding(){return pressedAndHoldingSomething;};
        const bool& getGamePaused(){return gamePaused;};
        const bool& getHoverOverEffect(){return hoverOverEffect;};
        const float& getMouseX()const{return mouseX;};
        const float& getMouseY()const{return mouseY;};
        UiManager& getUiManager(){return ui;};
        PhysicsCollider* getCurrentMouseCollider()const{return refColliderForMouseCurrent;};
        MinigameManager* getMinigameManager(){return minigameManager.get();};
        SoLoud::Soloud& getAudioEngine(){return audio;};

        //Setter
        void setFOV(const float& newFOV){fov = newFOV;};
        void setGamePaused(const bool& newPaused){gamePaused = newPaused;};
        void setInMenu(const bool& newMenu){inMenu = newMenu;};
        void setInLevelSelector(const bool& newLevelSelector){inLevelSelector = newLevelSelector;};
        void setMouseEntityManipulation(const bool& newM){entityManipulationThroughMouse = newM;};
        void setHoverOverEffect(const bool& newHover){hoverOverEffect = newHover;};
        void setHoverOverColor(const glm::vec4& newColor){hoverOverColor = newColor;};
        
        //Level Loading
        void loadMenu();
        void loadLevelSelector();
        void prepareForLevelChange();
        void resetMouseStates();
        void resetRegisterdFunctions();
        
        //Sound
        void playMusicByName(const std::string& musicName);
        void loadAllMusic();

        //Function Registering
        void registerFunctionToExecuteWhen(float whenFunctionShouldStart, std::function<void()> functionToExecute);
        void registerLoopingFunctionUntil(std::function<void()> functionToExecute, float secondsToRun);
        void registerRepeatingFunction(std::function<void()> functionToExecute, std::function<bool()> stopCondition);

};