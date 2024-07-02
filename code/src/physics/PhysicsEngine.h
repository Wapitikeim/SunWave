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
        float deltatime = 0;
        float tickTime = 0;
        
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister){physicsObjects.push_back(colliderToRegister);};
        void getInitialTransform(float _deltatime);
        void updatePhysics();

        void clearPhysicsObjects(){physicsObjects.clear();};

        float speedOfSimulation = 1;
        float tickrateOfSimulation = 150;
        float getTimeStep(){return (1/tickrateOfSimulation/(1/speedOfSimulation));};
};