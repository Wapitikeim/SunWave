#include "PhysicsEngine.h"

void PhysicsEngine::restoreInitialPosAndRot(PhysicsCollider* collider)
{
    for(auto test:initTransformOfColliders)
        {
            if(test.ref == collider)
            {
                collider->setPos(test.colliderInitialPos);
                collider->setRot(test.colliderInitialRot);
            }
        }
    
    //collider.setPos(colliderInitialPos);
    //collider.setRot(colliderInitialRot);
}

void PhysicsEngine::getInitialTransform(float _deltatime)
{
    deltatime = _deltatime;
    for(auto& obj:physicsObjects)
    {
        colliderInitialPos.push_back(obj->getBody().colliderPosition);
        colliderInitialRot.push_back(obj->getBody().colliderZRotation);
        
        initTransformOfColliders.push_back(PhysicColliderInitialTransform{obj,obj->getBody().colliderPosition, obj->getBody().colliderZRotation});
    }

}

void PhysicsEngine::updatePhysics()
{
    for(auto& obj:physicsObjects)
    {
        if(!obj->getIsStatic())
        {
            //ForcesApply
            obj->applyForce(glm::vec3(0,-9.81,0));
                

            //ApplyGravity
            //Friction ? NormalForce
            //Integrate
            obj->setVelocity(obj->getVelocity()+(obj->getAcceleration()*deltatime));
            obj->setPos(obj->getPos()+(obj->getVelocity()*deltatime));
            obj->setAcceleration(glm::vec3(0));
            
            //Collision Test
            for(auto& objectToTestTo : physicsObjects)
            {   
                if(objectToTestTo!=obj)
                {
                    if(CollisionTester::arePhysicsCollidersColliding(obj, objectToTestTo))
                    {
                        restoreInitialPosAndRot(obj);
                        
                        if((abs(obj->getVelocity().x) > 0.1f) || (abs(obj->getVelocity().y) > 0.1f))
                        {
                            //Poor mans attempt to trade forces
                            glm::vec3 velocityToTrade(-obj->getVelocity()*objectToTestTo->getElascicity());
                            obj->setVelocity(velocityToTrade);
                            if(!objectToTestTo->getIsStatic())
                                objectToTestTo->setVelocity(-velocityToTrade);
                        }
                        else
                        {
                            obj->setVelocity(glm::vec3(0));
                            obj->setIsGrounded(true);
                        }                
                    }   
                }  
            }
        }
        
    }
    initTransformOfColliders.clear();
}