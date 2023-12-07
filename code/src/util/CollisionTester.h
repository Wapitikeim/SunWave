#pragma once
#include <iostream>
#include <glm/glm.hpp> 
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include "../entities/Entity.h"

class CollisionTester
{
    private:
    static std::vector<glm::vec2> calcPointsWithRespectToRotation(Entity* entity);
    static std::vector<glm::vec2> calcProjectionFieldOutOfPoints(std::vector<glm::vec2> pointsToUse);
    static bool calcIfProjectionFieldIsOverlapping(glm::vec2 projectionField, std::vector<glm::vec2> pointsE1, std::vector<glm::vec2> pointsE2);


    public:
    static float signedDistance2DBox(glm::vec3 posToCheckTo, glm::vec3 posScale, glm::vec3 objectScale, glm::vec3 objectPos, float rotation);
    static bool simpleCheckForCollision(Entity* e1, Entity* e2);

    static bool areEntitiesColliding(Entity* e1, Entity* e2);
};