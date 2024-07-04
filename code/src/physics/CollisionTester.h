#pragma once
#include <iostream>
#include <glm/glm.hpp> 
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <array>
#include "../entities/Entity.h"
#include "../components/PhysicsCollider.h"

class CollisionTester
{
    private:
        static std::vector<glm::vec2> calcPointsWithRespectToRotation(PhysicsCollider* entity);
        static std::vector<glm::vec2> calcProjectionFieldOutOfPoints(std::vector<glm::vec2> pointsToUse);
        static bool calcIfProjectionFieldIsOverlapping(glm::vec2 projectionField, std::vector<glm::vec2> pointsE1, std::vector<glm::vec2> pointsE2);
        static bool simpleCheckForCollision(PhysicsCollider* e1, PhysicsCollider* e2);
        static bool simpleCollisionCheckBasedOnDistanceAndScale(PhysicsCollider* e1, PhysicsCollider* e2);
        static bool aAABCollisionCheck(PhysicsCollider* e1, PhysicsCollider* e2);
    public:
        //static float signedDistance2DBox(glm::vec3 posToCheckTo, glm::vec3 posScale, glm::vec3 objectScale, glm::vec3 objectPos, float rotation);
        static float signedDistancePointAnd2DBox(glm::vec3 pointPos, PhysicsCollider* colliderToCheck);
        static float signedDistance2DBoxAnd2DBox(PhysicsCollider* colliderToCheckOne, PhysicsCollider* colliderToCheckTwo);                

        static bool arePhysicsCollidersColliding(PhysicsCollider* e1, PhysicsCollider* e2);
};