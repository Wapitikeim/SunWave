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
    glm::vec3 colliderScale;
    float colliderZRotation;
    float mass;
};

class PhysicsCollider : public Component
{
    private:
        
        PhysicsBody colliderBody;
        Entity* entityThisIsAttachedTo;
        bool isStatic = true;
        
    protected:
        
    public:
        PhysicsCollider(Entity* entityToAttachTo, bool _isStatic):entityThisIsAttachedTo(entityToAttachTo),isStatic(_isStatic)
        {
            this->componentName = "Physics";
            update();
            colliderBody.mass = 1.f;
            colliderBody.colliderVelocity = glm::vec3(0.);
        };

        [[nodiscard]] const PhysicsBody &getBody()const{return colliderBody;};
        void setBody(const PhysicsBody &newBody);
        
        void update() override;

        

};