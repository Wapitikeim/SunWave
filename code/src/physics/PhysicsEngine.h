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
        bool isHalting = false;
        
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister)
        {
            if(colliderToRegister == nullptr)
            {
                std::cout << "Tryed to insert a nullptr";
                return;
            }
            for(auto& entry: physicsObjects)
                if(entry == colliderToRegister)
                {
                    std::cout << "Tryed to register a already existing Physicscollider: " << colliderToRegister->getNameOfEntityThisIsAttachedTo() << "\n";
                    return;
                }
            std::cout << "Registering the collider of " << colliderToRegister->getNameOfEntityThisIsAttachedTo() << " to the Physics Engine. \n";        
            physicsObjects.push_back(colliderToRegister);
        };
        void unregisterCollider(PhysicsCollider* colliderToRemove)
        {
            if(colliderToRemove == nullptr)
            {
                std::cout << "Tryed to remove a nullptr from the Physics Engine \n";
                return;
            }
                
            removeColliderFromHashTable(colliderToRemove);
            for(auto entry:physicsObjects)
                physicsObjects.erase(std::remove(physicsObjects.begin(), physicsObjects.end(), colliderToRemove),physicsObjects.end());
            std::cout << "Removed Physicscollider form " << colliderToRemove->getNameOfEntityThisIsAttachedTo() << " from the Physics engine\n";
        };
        void getInitialTransform(float _deltatime);
        void updatePhysics();

        void clearPhysicsObjects(){physicsObjects.clear();hashTable.clear();initDone = false;};

        float speedOfSimulation = 1;
        float tickrateOfSimulation = 150;
        float getTimeStep(){return (1/tickrateOfSimulation/(1/speedOfSimulation));};

        void setcameraXHalf(float &newXHalf){cameraXHalf = newXHalf;};
        void setcameraYHalf(float &newYHalf){cameraYHalf = newYHalf;};

        void setIsHalting(const bool &newHalt){isHalting = newHalt;};
        bool getIsHalting(){return isHalting;};  
};