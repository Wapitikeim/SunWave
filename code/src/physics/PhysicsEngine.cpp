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
    tickTime += _deltatime;
    for(auto& obj:physicsObjects)
    {
        colliderInitialPos.push_back(obj->getBody().colliderPosition);
        colliderInitialRot.push_back(obj->getBody().colliderZRotation);
        
        initTransformOfColliders.push_back(PhysicColliderInitialTransform{obj,obj->getBody().colliderPosition, obj->getBody().colliderZRotation});
    }

}

void PhysicsEngine::updatePhysics()
{
    while (tickTime >= getTimeStep())
    {
        for(auto& collider:physicsObjects)
        {
            if(collider->getIsTrigger())
                continue;
            
            if(!collider->getIsStatic())
            {
                //ForcesApply
                if(collider->getVelocity().y >0)
                    collider->setIsGrounded(false);
                if(!collider->getIsGrounded())
                    collider->applyForce(glm::vec3(0,-9.81,0));
                    

                //ApplyGravity
                //Friction ? NormalForce
                //Integrate
                collider->setVelocity(collider->getVelocity()+(collider->getAcceleration()*getTimeStep()));
                collider->setPos(collider->getPos()+(collider->getVelocity()*getTimeStep()));
                collider->setAcceleration(glm::vec3(0));
                
                //Collision Test
                for(auto& colliderToCheckCollisionFor : physicsObjects)
                {   
                    if(colliderToCheckCollisionFor!=collider && !colliderToCheckCollisionFor->getIsTrigger())
                    {
                        if(CollisionTester::arePhysicsCollidersColliding(collider, colliderToCheckCollisionFor))
                        {
                            restoreInitialPosAndRot(collider);
                            collider->setColliderThisIsInContactWith(colliderToCheckCollisionFor);
                            if((abs(collider->getVelocity().x) > 0.1f) || (abs(collider->getVelocity().y) > 0.1f))
                            {
                                //Poor mans attempt to trade forces
                                glm::vec3 velocityToTrade(-collider->getVelocity()*colliderToCheckCollisionFor->getElascicity());
                                collider->setVelocity(velocityToTrade);
                                if(!colliderToCheckCollisionFor->getIsStatic())
                                    colliderToCheckCollisionFor->setVelocity(-velocityToTrade);
                            }
                            else
                            {
                                collider->setVelocity(glm::vec3(0));
                                collider->setIsGrounded(true);
                            }                
                        }
                        collider->removeColliderFromContactList(colliderToCheckCollisionFor);   
                    }  
                }
            }
            
        }
        tickTime -= (1/tickrateOfSimulation);
    }
    initTransformOfColliders.clear();
    
}