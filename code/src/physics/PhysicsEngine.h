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
    float colliderInitialRot; 
};

class PhysicsEngine 
{
    private:
        //Colliders to apply physics to
        std::vector<PhysicsCollider*> physicsObjects;
        
        //Forces
        float GRAVITY = -9.81f;
        float AIR_RESISTANCE = 0.995f;
        void updatePhysicsState();
        void applyGravity(PhysicsCollider* collider);
        void integrateForces(PhysicsCollider* collider);

        //CollisionDetection
        std::vector<std::vector<PhysicsCollider*>> collisionsToResolve;
        //---Morton encoded hash Grid 
        std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> mortonHashTable;
        MortonHashTableLogic tableLogic;
        void collisionDetection();
        void broadCollisionGathering();
        void narrowCollisionGathering();
        
        //CollisionResponse
        float RESTING_THRESHOLD = 0.15f;
        float BOUNCE_MULTIPLIER = 2.25f;
        std::vector<glm::vec3> colliderInitialPos;
        std::vector<float> colliderInitialRot;
        std::vector<PhysicColliderInitialTransform> initTransformOfColliders;
        void collisionRespone();
        void resolveCollision(PhysicsCollider* colliderA, PhysicsCollider* colliderB, const glm::vec3& contactNormal, float penetrationDepth, const glm::vec3& relativeVelocity);
        void restoreInitialPosAndRot(PhysicsCollider* collider);
        
        //Debug/Info Helpers
        int maxCollisionsResolvedLastTick = 0;
        int ticksLastFrame = 0;
        int ticksBuffer = 0;
        int ticksCalculatedInOneSecond = 0;
        float cameraXHalf;
        float cameraYHalf;
        float deltatime = 0;
        float tickTime = 0;
        bool initDone = false;
        bool isHalting = false;
        void updateNonCollidingColliders();
        
    public:
        
        //Main update for physics update
        void updatePhysics();
        //Management of PhysicsColliders
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
        void clearPhysicsObjects()
        {
            mortonHashTable.clear();
            collisionsToResolve.clear();
            initTransformOfColliders.clear();
            colliderInitialPos.clear();
            colliderInitialRot.clear();
            physicsObjects.clear(); 
            physicsObjects.shrink_to_fit();
            initDone = false;  
        };
        bool getPhysicsObjectsEmpty(){return physicsObjects.empty();};

        //Physics Engine control
        float speedOfSimulation = 1;
        float tickrateOfSimulation = 150;
        //Getter
        float getTimeStep(){return (1.0f / tickrateOfSimulation) * speedOfSimulation;};
        bool getIsHalting(){return isHalting;};
        const int& getCurrentCollisions(){return maxCollisionsResolvedLastTick;};
        const int& getTicksLastFrame(){return ticksLastFrame;};
        const int& getTicksLastSecond(){return ticksCalculatedInOneSecond;};
        const float& getBounceMultiplier(){return BOUNCE_MULTIPLIER;};
        const std::vector<PhysicsCollider*>& getActiveColliders(){return physicsObjects;};
        const bool& getInitDone()const {return initDone;};
        void getInitialTransform(float _deltatime);
        //Setter
        void setcameraXHalf(float &newXHalf){cameraXHalf = newXHalf;};
        void setcameraYHalf(float &newYHalf){cameraYHalf = newYHalf;};
        void setIsHalting(const bool &newHalt){isHalting = newHalt; mortonHashTable.clear(); initDone=false;tickTime=0;};
        void setInitDone(const bool& newInit){initDone = newInit;};
        void setBounceMultiplier(const float &newBounce){BOUNCE_MULTIPLIER = newBounce;};
        
        //HasTableFunctions
        void addColliderIntoHashTable(PhysicsCollider* colliderRef);
        void removeColliderFromHashTable(PhysicsCollider* colliderRef);
        int getHashTableIndicesSize();

        //Collision functions meant to be used outside
        bool checkIfShellWouldCollide(glm::vec3 &pos, glm::vec3 &scale, float &rotZ);
        bool checkTriggerColliderCollision(const std::string& colliderEntityName, const std::string& triggerColliderEntityName);
        bool checkColliderPlayerCollision(const std::string& colliderEntityName);
        PhysicsCollider* getFirstColliderShellCollidesWith(glm::vec3 &pos, glm::vec3 &scale, float &rotZ);
};