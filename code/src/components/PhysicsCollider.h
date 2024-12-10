#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include "../entities/Entity.h"
//#include "../physics/PhysicsEngine.h"

struct PhysicsBody
{
    //Transform
    glm::vec3 colliderPosition;
    glm::vec3 colliderScale;
    float colliderZRotation;
    //Physics
    glm::vec3 colliderVelocity;
    glm::vec3 colliderAcceleration;
    float mass;
};

struct CornerPositions
{
    //Test for holding the CornerPositions of Boxes
    glm::vec3 leftBottom;
    glm::vec3 leftTop;
    glm::vec3 rightTop;
    glm::vec3 rightBottom;
    std::vector<glm::vec3> cornerVec;
};

class PhysicsCollider : public Component
{
    private:
        bool isGrounded = false;
        bool isStatic = true;
        bool isTrigger = false;
        bool isResting = false;
        float elasticity = 0.2; // 0 = perfectly inalastic 1 = perfectly elastic
        
        PhysicsBody colliderBody;
        std::vector<PhysicsCollider*> isInContactWith;
        Entity* entityThisIsAttachedTo;

        void updateEntityPosAndRot();

        CornerPositions cornerPos;
        void updateCornerPositions();

        std::vector<uint32_t> indiciesInHashTable;
        
    protected:

    public:
        PhysicsCollider(Entity* entityToAttachTo, bool _isStatic):entityThisIsAttachedTo(entityToAttachTo),isStatic(_isStatic)
        {
            this->componentName = "Physics";
            colliderBody.mass = 1.f;
            colliderBody.colliderVelocity = glm::vec3(0);
            colliderBody.colliderAcceleration = glm::vec3(0);
            if(isStatic)
                elasticity = 0.5f;
            update();
            updateCornerPositions(); // That static entites getting thier Corner Positions at least once
            
        };

        [[nodiscard]] const PhysicsBody &getBody()const{return colliderBody;};
        [[nodiscard]] const glm::vec3 &getPos()const{return colliderBody.colliderPosition;};
        [[nodiscard]] const glm::vec3 &getVelocity()const{return colliderBody.colliderVelocity;};
        [[nodiscard]] const glm::vec3 &getAcceleration()const{return colliderBody.colliderAcceleration;};
        [[nodiscard]] const float &getRot()const{return colliderBody.colliderZRotation;};
        
        [[nodiscard]] const bool &getIsStatic()const{return isStatic;};
        [[nodiscard]] const bool &getIsTrigger()const{return isTrigger;};
        [[nodiscard]] const bool &getIsResting()const{return isResting;};
        [[nodiscard]] const bool &getIsGrounded()const{return isGrounded;};
        [[nodiscard]] const float &getElascicity()const{return elasticity;};
        [[nodiscard]] const float &getMass()const{return colliderBody.mass;};
        [[nodiscard]] const std::vector<uint32_t> &getTableIndicies()const{return indiciesInHashTable;};

        void setBody(const PhysicsBody &newBody){colliderBody = newBody; updateEntityPosAndRot();};
        void setPos(const glm::vec3 &newPos){colliderBody.colliderPosition = newPos; updateEntityPosAndRot();};
        void setVelocity(const glm::vec3 &newVelocity){colliderBody.colliderVelocity = newVelocity;};
        void setAcceleration(const glm::vec3 &newAcceleration){colliderBody.colliderAcceleration = newAcceleration;};
        void setRot(const float &newRot){colliderBody.colliderZRotation = newRot; updateEntityPosAndRot();};
        void setIsGrounded(const bool &newGround){isGrounded = newGround;};
        void setIsTrigger(const bool &newTrigger){isTrigger = newTrigger;};
        void setIsResting(const bool &newResting){isResting = newResting;};
        void setIsStatic(const bool &newStatic){isStatic = newStatic;};
        void setElascity(const float &newElascicity){elasticity = newElascicity;};
        void setMass(const float &newMass){colliderBody.mass = newMass;};
        void applyForce(const glm::vec3 direction){colliderBody.colliderAcceleration+=direction*(1/colliderBody.mass);};
        void setIndiciesForHashTable(const std::vector<uint32_t> &newIndicies){indiciesInHashTable = newIndicies;};
        void addOneIndexIntoIndiciesForHashTable(const uint32_t &index)
        {
            for(auto& entry: indiciesInHashTable)
                if(entry==index)
                        return; 
            indiciesInHashTable.push_back(index);
        };
        void clearIndiciesForHashTableOfThisEntity(){indiciesInHashTable.clear();};
        std::vector<PhysicsCollider*> getTheColliderThisColliderIsInContactWith(){return isInContactWith;};
        std::vector<glm::vec3> getCornerPosAsVector(){return cornerPos.cornerVec;};
        void setColliderThisIsInContactWith(PhysicsCollider* collider)
        {
            int checkBeforeAdd = std::count(isInContactWith.begin(),isInContactWith.end(),collider);
            if(!checkBeforeAdd)
                isInContactWith.push_back(collider);
        };
        void removeColliderFromContactList(PhysicsCollider* collider)
        {
            auto element = std::find(isInContactWith.begin(),isInContactWith.end(),collider);
            if(element != isInContactWith.end())
                isInContactWith.erase(element);
        };
        bool isGivenColliderInContactWithThisCollider(PhysicsCollider* collider){return std::count(isInContactWith.begin(),isInContactWith.end(),collider);};
        int getSizeOfContacts(){return isInContactWith.size();};

        void update() override;

        std::string getNameOfEntityThisIsAttachedTo(){return entityThisIsAttachedTo->getEntityName();};

        CornerPositions& getCornerPos(){return cornerPos;};

        Entity* getEntityThisIsAttachedTo(){return entityThisIsAttachedTo;};
};