#pragma once

#include "Component.h"
#include <glm/glm.hpp>
#include <string>
#include "../entities/Entity.h"
//#include "../physics/PhysicsEngine.h"

struct PhysicsBody
{
    glm::vec3 colliderPosition;
    glm::vec3 colliderVelocity;
    glm::vec3 colliderAcceleration;
    glm::vec3 colliderScale;
    float colliderZRotation;
    float mass;
};

class PhysicsCollider : public Component
{
    private:
        
        PhysicsBody colliderBody;
        
        bool isGrounded = false;
        bool isStatic = true;
        float elasticity = 0.2; // 0 = perfectly inalastic 1 = perfectly elastic
        
        Entity* entityThisIsAttachedTo;

        void updateEntityPosAndRot();
        
    protected:

    public:
        PhysicsCollider(Entity* entityToAttachTo, bool _isStatic):entityThisIsAttachedTo(entityToAttachTo),isStatic(_isStatic)
        {
            this->componentName = "Physics";
            update();
            colliderBody.mass = 1.f;
            colliderBody.colliderVelocity = glm::vec3(0);
            colliderBody.colliderAcceleration = glm::vec3(0);
            if(isStatic)
                elasticity = 0.5f;
        };

        [[nodiscard]] const PhysicsBody &getBody()const{return colliderBody;};
        [[nodiscard]] const glm::vec3 &getPos()const{return colliderBody.colliderPosition;};
        [[nodiscard]] const glm::vec3 &getVelocity()const{return colliderBody.colliderVelocity;};
        [[nodiscard]] const glm::vec3 &getAcceleration()const{return colliderBody.colliderAcceleration;};
        [[nodiscard]] const float &getRot()const{return colliderBody.colliderZRotation;};
        
        [[nodiscard]] const bool &getIsStatic()const{return isStatic;};
        [[nodiscard]] const bool &getIsGrounded()const{return isGrounded;};
        [[nodiscard]] const float &getElascicity()const{return elasticity;};

        void setBody(const PhysicsBody &newBody){colliderBody = newBody; updateEntityPosAndRot();};
        void setPos(const glm::vec3 &newPos){colliderBody.colliderPosition = newPos; updateEntityPosAndRot();};
        void setVelocity(const glm::vec3 &newVelocity){colliderBody.colliderVelocity = newVelocity;};
        void setAcceleration(const glm::vec3 &newAcceleration){colliderBody.colliderAcceleration = newAcceleration;};
        void setRot(const float &newRot){colliderBody.colliderZRotation = newRot; updateEntityPosAndRot();};
        void setIsGrounded(const bool &newGround){isGrounded = newGround;};
        void setElascity(const float &newElascicity){elasticity = newElascicity;};
        void applyForce(const glm::vec3 direction){colliderBody.colliderAcceleration+=direction;};
        
        void update() override;

        std::string getNameOfEntityThisIsAttachedTo(){return entityThisIsAttachedTo->getEntityName();};

};