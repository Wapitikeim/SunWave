#pragma once 
#include "Entity.h"
#include "../components/PhysicsCollider.h"


class PlayerShape: public Entity
{
    private:
        //Mesh
        std::shared_ptr<std::vector<float>> quadrat;
        std::shared_ptr<std::vector<int>> EBOIndices;
        const float BASE_VELOCITY = 10.0f; 
        const float FORCE_MULTIPLIER = 800.0f;  
    public:
        PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, std::string texturePath);
        PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, bool letFragmentShaderDraw , std::string fragmentShapeName);
        void update(GLFWwindow* window, float deltaTime) override;
        float velocity = BASE_VELOCITY;
};