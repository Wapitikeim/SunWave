#include "InfiniteGrid.h"

void InfiniteGrid::setupGrid()
{
    std::shared_ptr<std::vector<float>> gridVertex = std::make_shared<std::vector<float>>();
    gridVertex->assign({
        1.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,

    
    });
    gridIndices = std::make_shared<std::vector<int>>();
    gridIndices->assign({ 
        0,1,2,
        2,0,3
    });

    // Grid VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //Grid VBO
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertex->capacity()*4, &gridVertex->operator[](0), GL_STATIC_DRAW);

    //Grid Vertex Attribute Interpretation
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    //Grid EBO
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, gridIndices->capacity()*4, &gridIndices->operator[](0), GL_STATIC_DRAW);
}

void InfiniteGrid::drawGrid(glm::mat4 viewMatForGrid, glm::mat4 projMatForGrid, float scale)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gridShader.useShader();
    gridShader.setUniformMat4("view", viewMatForGrid);
    gridShader.setUniformMat4("projection", projMatForGrid);
    gridShader.setUniformFloat("scale", scale);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, gridIndices->capacity(), GL_UNSIGNED_INT, 0);
}

void InfiniteGrid::drawGrid(float scale)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    gridShader.useShader();
    gridShader.setUniformMat4("view", Camera::getCurrentCameraView());
    gridShader.setUniformMat4("projection", Camera::getCurrentCameraProjection());
    gridShader.setUniformFloat("scale", scale);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, gridIndices->capacity(), GL_UNSIGNED_INT, 0);
}
