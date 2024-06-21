#pragma once 
#include <glm/glm.hpp>

#include "CollisionTester.h"
#include "../components/PhysicsCollider.h"


class PhysicsEngine 
{
    private:
        std::vector<glm::vec3> entityInitialPos;
        std::vector<float> entityInitialRot;
        std::vector<PhysicsCollider*> physicsObjects;
        //void isCollisionDetected();
        //void resolveCollision();
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister){physicsObjects.push_back(colliderToRegister);};
        void updateInfo();
        void updatePhysics();
};