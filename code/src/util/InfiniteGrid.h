#pragma once

#include "../shaders/ShaderContainer.h"
#include "Camera.h"
#include <glm/vec4.hpp>

//Currently just xz Grid
class InfiniteGrid
{
    private:
        ShaderContainer gridShader;
        std::shared_ptr<std::vector<int>> gridIndices;
        
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        
        void setupGrid();

    public:
        InfiniteGrid(ShaderContainer gridShaderToPass) 
        :gridShader(std::move(gridShaderToPass))
        {
            setupGrid();
        };

        void drawGrid(glm::mat4 viewMatForGrid, glm::mat4 projMatForGrid, float scale);
        void drawGrid(float scale);
};