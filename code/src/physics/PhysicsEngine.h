#pragma once 
#include <glm/glm.hpp>
#include <map>
#include <algorithm>

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

        //Spitial Hash Grid (Maybe in another class later)
        int spacing = 4; // double the size of standard collider 
        float cameraXHalf;
        float cameraYHalf;
        std::map<int, std::vector<PhysicsCollider*>> hashTable; //<index,colliderRef>
        std::vector<int> indiciesForHashTable;
        std::vector<glm::vec2> pointsToGetIndexesFor;

        void addColliderIntoHashTable(PhysicsCollider* colliderRef);
        void removeColliderFromHashTable(PhysicsCollider* colliderRef);
        void addColliderNeighboursAlso(PhysicsCollider* colliderRef);
        //std::vector<PhysicsCollider&> getNearEntitysFromHashTable(){return;};
        
        std::vector<std::vector<PhysicsCollider*>> collisionsToResolve;

        bool initDone = false;
        
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister)
        {
            physicsObjects.push_back(colliderToRegister);
        };
        void getInitialTransform(float _deltatime);
        void updatePhysics();

        void clearPhysicsObjects(){physicsObjects.clear();};

        float speedOfSimulation = 1;
        float tickrateOfSimulation = 150;
        float getTimeStep(){return (1/tickrateOfSimulation/(1/speedOfSimulation));};

        void setcameraXHalf(float &newXHalf){cameraXHalf = newXHalf;};
        void setcameraYHalf(float &newYHalf){cameraYHalf = newYHalf;};  
};