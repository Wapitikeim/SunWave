#pragma once 
#include <glm/glm.hpp>

#include "CollisionTester.h"
#include "../components/PhysicsCollider.h"

struct PhysicColliderInitialTransform
{
    int index;
    std::vector<glm::vec3> colliderInitialPos;
    std::vector<float> colliderInitialRot;
};


class PhysicsEngine 
{
    private:
        std::vector<glm::vec3> colliderInitialPos;
        std::vector<float> colliderInitialRot;
        std::vector<PhysicsCollider*> physicsObjects;
        //void isCollisionDetected();
        //void resolveCollision();
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister){physicsObjects.push_back(colliderToRegister);};
        void getInitialTransform();
        void updatePhysics();
};