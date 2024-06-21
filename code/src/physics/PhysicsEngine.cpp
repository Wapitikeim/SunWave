#include "PhysicsEngine.h"

void PhysicsEngine::getInitialTransform()
{
    for(auto& obj:physicsObjects)
    {
        colliderInitialPos.push_back(obj->getBody().colliderPosition);
        colliderInitialRot.push_back(obj->getBody().colliderZRotation);
    }
}

void PhysicsEngine::updatePhysics()
{
    for(auto& obj:physicsObjects)
    {
        if(!obj->getIsStatic())
        {
            for(int i = 1; i < physicsObjects.size(); i++)
            {
                if(CollisionTester::arePhysicsCollidersColliding(obj, physicsObjects[i]))
                {
                    obj->setPos(colliderInitialPos[0]);
                    obj->setRot(colliderInitialRot[0]);
                }
            }
        }
        colliderInitialPos.clear();
        colliderInitialRot.clear();
    }
}