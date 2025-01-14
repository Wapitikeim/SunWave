#pragma once 
class GameEnvironment;

//Imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"



class UiManager
{
    private:
        GameEnvironment* gameEnv;
        GLFWwindow* window;
        
        //std::vector<std::unique_ptr<Entity>>& entities;
        //PhysicsEngine* physicsEngine;

        bool showGrid=true;
        float gridSize = 1.f;
        int imGuiFPS = 0;


        void setupImGui();


        void drawImGuiControlPanel();
        bool imGuiControlPanel = true;
        void drawImGuiWorldControl();
        bool worldControl = true;
        void drawImGuiInfoPanel();
        bool infoPanel = true;
        void drawImGuiLevelManager();
        bool levelManager = true;
        void drawImGuiPhysicsEngineControl();
        bool physicsEngineControl = true;
        void drawImGuiPlayerExtraInfo();
        bool playerExtraInfo = false; 
        void drawImGuiMouseInformation();
        bool mouseInformation = false;
        void drawImGuiEnitityAdder();
        bool entityAdder = true;

        
    public:
        UiManager(GLFWwindow* w, GameEnvironment* gE)
        :window(w),gameEnv(gE)
        {
            setupImGui();
        }
        ~UiManager()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        const bool& getShowGrid() const {return showGrid;};
        void setShowGrid(const bool& newShow){showGrid = newShow;};

        const float& getGridSize() const {return gridSize;};
        void setGridSize(const float& newGridSize) {gridSize=newGridSize;};

        const int& getImGuiFps() const {return imGuiFPS;};
        void setImGuiFps(const int& newFPS) {imGuiFPS=newFPS;};
        
        void prepFrames();
        void draw();
};