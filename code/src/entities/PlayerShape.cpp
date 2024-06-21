#include "PlayerShape.h"



PlayerShape::PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, std::string texturePath) 
: Entity(std::move(shapeName), ShaderContainer("defaultVertexShader.glsl", "defaultFragmentShader.glsl"), MeshContainer(nullptr, nullptr, texturePath), shapePosition, shapeScale, shapeRotation)
{
    //Default Mesh Coord
    quadrat = std::make_shared<std::vector<float>>();
    quadrat->assign(
    {
    // positions          // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
    }
    );
    EBOIndices = std::make_shared<std::vector<int>>();
    EBOIndices->assign({  // note that we start from 0!
    0, 1, 2,   // first triangle
    3, 2, 0    // second triangle
    });
    entityMesh.vertexAttributes = quadrat;
    entityMesh.indicesForEBO = EBOIndices;
    entityMesh.reinitMesh();
}

PlayerShape::PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, bool letFragmentShaderDraw, std::string fragmentShapeName)
: Entity(std::move(shapeName), ShaderContainer("defaultVertexShader.glsl", "shapeFragmentShader/"+fragmentShapeName+".glsl"), MeshContainer(nullptr, nullptr, "DefaultTexture.png"), shapePosition, shapeScale, shapeRotation)
{
    //Default Mesh Coord
    quadrat = std::make_shared<std::vector<float>>();
    quadrat->assign(
    {
    // positions          // texture coords
     1.0f,  1.0f, 0.0f,   1.0f, 1.0f,   // top right
     1.0f, -1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
    -1.0f, -1.0f, 0.0f,   0.0f, 0.0f,   // bottom left
    -1.0f,  1.0f, 0.0f,   0.0f, 1.0f    // top left 
    }
    );
    EBOIndices = std::make_shared<std::vector<int>>();
    EBOIndices->assign({  // note that we start from 0!
    0, 1, 2,   // first triangle
    3, 2, 0    // second triangle
    });
    entityMesh.vertexAttributes = quadrat;
    entityMesh.indicesForEBO = EBOIndices;
    entityMesh.reinitMesh();
}

void PlayerShape::update(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
        {
            entityPosition.x -= velocity * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
        {
            entityPosition.x += velocity * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            entityPosition.y += velocity * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            entityPosition.y -= velocity * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            entityRotation += 40.0f*deltaTime;
            if(entityRotation > 360)
                entityRotation = 0;
        }   
    for(auto &comp:this->componentsOfThisEntity)
    {
        comp->update();
    }
}

