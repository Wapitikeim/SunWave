#include "PlayerShape.h"



PlayerShape::PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, std::string texturePath) 
: Entity(std::move(shapeName), ShaderContainer("defaultVertexShader.glsl", "defaultFragmentShader.glsl"), MeshContainer(nullptr, nullptr, texturePath), shapePosition, shapeScale, shapeRotation)
{
    type = "PlayerShape";
    shaderName = texturePath;
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
    type = "PlayerShape";
    shaderName = fragmentShapeName;
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
    auto* physics = dynamic_cast<PhysicsCollider*>(getComponent("Physics"));
    if (!physics) return;

    // Scale force by deltaTime and normalize to 60fps
    float frameAdjustedForce = velocity * deltaTime * FORCE_MULTIPLIER;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) 
    {
        physics->applyForce(glm::vec3(-frameAdjustedForce, 0, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) 
    {
        physics->applyForce(glm::vec3(frameAdjustedForce, 0, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) 
    {
        physics->applyForce(glm::vec3(0, frameAdjustedForce, 0));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) 
    {
        physics->applyForce(glm::vec3(0, -frameAdjustedForce, 0));
    }  
    
    for(auto &comp:this->componentsOfThisEntity)
    {
        comp->update();
    }
}

