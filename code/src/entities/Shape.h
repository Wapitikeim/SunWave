#pragma once 
#include "Entity.h"


class Shape: public Entity
{
    private:
    //MeshStuff
    std::shared_ptr<std::vector<float>> quadrat;
    
    std::shared_ptr<std::vector<int>> EBOIndices;

    public:
    Shape(std::string shapeName, glm::vec3 shapePosition, glm::vec3 shapeScale, float shapeRotation, std::string texturePath);
};