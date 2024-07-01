#pragma once 
#include <glm/glm.hpp>

#include "CollisionTester.h"
#include "../components/PhysicsCollider.h"

struct PhysicColliderInitialTransform
{
    PhysicsCollider* ref;
    glm::vec3 colliderInitialPos;
    float colliderInitialRot;
};


class PhysicsEngine 
{
    private:
        std::vector<glm::vec3> colliderInitialPos;
        std::vector<float> colliderInitialRot;

        std::vector<PhysicColliderInitialTransform> initTransformOfColliders;
        std::vector<PhysicsCollider*> physicsObjects;
        //void isCollisionDetected();
        //void resolveCollision();
        void restoreInitialPosAndRot(PhysicsCollider* collider);
        float deltatime;
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister){physicsObjects.push_back(colliderToRegister);};
        void getInitialTransform(float _deltatime);
        void updatePhysics();
};