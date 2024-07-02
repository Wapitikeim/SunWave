#pragma once 
#include "Entity.h"
#include "../components/PhysicsCollider.h"


class PlayerShape: public Entity
{
    private:
        //MeshStuff
        std::shared_ptr<std::vector<float>> quadrat;
        
        std::shared_ptr<std::vector<int>> EBOIndices;

        

    public:
        PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, std::string texturePath);
        PlayerShape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, bool letFragmentShaderDraw , std::string fragmentShapeName);
        void update(GLFWwindow* window, float deltaTime) override;
        float velocity = 10.f; // 7.5
};