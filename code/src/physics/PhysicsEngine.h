#pragma once 
#include <glm/glm.hpp>
#include <map>
#include <unordered_map>
#include <algorithm>


#include "CollisionTester.h"
#include "MortonHashTableLogic.h"
#include "../components/PhysicsCollider.h"

struct PhysicColliderInitialTransform
{
    PhysicsCollider* ref;
    glm::vec3 colliderInitialPos;
    //glm::vec3 colliderInitialVelocity;
    float colliderInitialRot;
    
};

class PhysicsEngine 
{
    private:
        float deltatime = 0;
        float tickTime = 0;
        std::vector<glm::vec3> colliderInitialPos;
        std::vector<float> colliderInitialRot;

        std::vector<PhysicColliderInitialTransform> initTransformOfColliders;
        std::vector<PhysicsCollider*> physicsObjects;
        void restoreInitialPosAndRot(PhysicsCollider* collider);
        

        //Spitial Hash Grid 
        std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> mortonHashTable;
        MortonHashTableLogic tableLogic;
        void testing();

        int spacing = 4; // double the size of standard collider 
        float cameraXHalf;
        float cameraYHalf;

        
        //std::vector<PhysicsCollider&> getNearEntitysFromHashTable(){return;};
        
        std::vector<std::vector<PhysicsCollider*>> collisionsToResolve;
        
        //PhysicsUpdateLoop
        void updatePhysicsState();
        void applyGravity(PhysicsCollider* collider);
        float GRAVITY = -9.81f;
        float AIR_RESISTANCE = 0.995f;
        void integrateForces(PhysicsCollider* collider);
        void broadCollisionGathering();
        void updateNonCollidingColliders();
        void narrowCollisionGathering();
        void collisionDetection();
        float RESTING_THRESHOLD = 0.05f;
        float BOUNCE_MULTIPLIER = 2.25f;
        void collisionRespone();
        void resolveCollision(PhysicsCollider* colliderA, PhysicsCollider* colliderB, const glm::vec3& contactNormal, float penetrationDepth, const glm::vec3& relativeVelocity);

        
        //Debug/Info Helpers
        int maxCollisionsResolvedLastTick = 0;
        int ticksLastFrame = 0;
        int ticksBuffer = 0;
        int ticksCalculatedInOneSecond = 0;

        bool initDone = false;
        bool isHalting = false;
        
    public:
        void registerPhysicsCollider(PhysicsCollider* colliderToRegister)
        {
            if(colliderToRegister == nullptr)
            {
                std::cout << "Tryed to insert a nullptr into the Physics Engine \n";
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
            addColliderIntoHashTable(colliderToRegister);
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

        void clearPhysicsObjects()
        {
            mortonHashTable.clear();
            collisionsToResolve.clear();
            initTransformOfColliders.clear();
            colliderInitialPos.clear();
            colliderInitialRot.clear();
            physicsObjects.clear(); 
            initDone = false;  
        };

        float speedOfSimulation = 1;
        float tickrateOfSimulation = 150;
        float getTimeStep(){return (1.0f / tickrateOfSimulation) * speedOfSimulation;};

        void setcameraXHalf(float &newXHalf){cameraXHalf = newXHalf;};
        void setcameraYHalf(float &newYHalf){cameraYHalf = newYHalf;};

        void setIsHalting(const bool &newHalt){isHalting = newHalt; mortonHashTable.clear(); initDone=false;tickTime=0;};
        bool getIsHalting(){return isHalting;};
        const int& getCurrentCollisions(){return maxCollisionsResolvedLastTick;};
        const int& getTicksLastFrame(){return ticksLastFrame;};
        const int& getTicksLastSecond(){return ticksCalculatedInOneSecond;};
        const float& getBounceMultiplier(){return BOUNCE_MULTIPLIER;};
        const std::vector<PhysicsCollider*>& getActiveColliders(){return physicsObjects;};
        void setInitDone(const bool& newInit){initDone = newInit;};
        void setBounceMultiplier(const float &newBounce){BOUNCE_MULTIPLIER = newBounce;};
        bool checkIfShellWouldCollide(glm::vec3 &pos, glm::vec3 &scale, float &rotZ);
        PhysicsCollider* getFirstColliderShellCollidesWith(glm::vec3 &pos, glm::vec3 &scale, float &rotZ);

        //HasTableFunctions
        void addColliderIntoHashTable(PhysicsCollider* colliderRef);
        void removeColliderFromHashTable(PhysicsCollider* colliderRef);
        int getHashTableIndicesSize();
};