#include "PhysicsEngine.h"
#include <bitset>

void PhysicsEngine::restoreInitialPosAndRot(PhysicsCollider* collider)
{
    for(auto entry:initTransformOfColliders)
        {
            if(entry.ref == collider)
            {
                collider->setPos(entry.colliderInitialPos);
                collider->setRot(entry.colliderInitialRot);
                collider->updateCornerPositions();
            }
        }
    
    //collider.setPos(colliderInitialPos);
    //collider.setRot(colliderInitialRot);
}

void PhysicsEngine::testing()
{
    if (collisionsToResolve.size() == 0)
        return;

    //std::cout << "Collisions to Resolve:" << std::endl;
    for (const auto& collisionList : collisionsToResolve)
    {
        //std::cout << "Collision Group:" << std::endl;
        for (const auto& collider : collisionList)
        {
            //std::cout << "  Collider: " << collider->getNameOfEntityThisIsAttachedTo() << std::endl;
            // Change color to red if colliding
            collider->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange", glm::vec4(1, 0, 0, 1));
        }
    }
    //std::cout << "\n";

    // Set color to black for colliders not in collisionsToResolve
    for (const auto& collider : physicsObjects)
    {
        bool isColliding = false;
        for (const auto& collisionList : collisionsToResolve)
        {
            if (std::find(collisionList.begin(), collisionList.end(), collider) != collisionList.end())
            {
                isColliding = true;
                break;
            }
        }
        if (!isColliding)
        {
            collider->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange", glm::vec4(0, 0, 0, 1));
        }
    }
}

void PhysicsEngine::applyForces()
{
    //Force Apply + Update
    for(auto& collider:physicsObjects)
    {
        if(collider->getIsTrigger() || collider->getIsStatic()&&(collider->getVelocity()==glm::vec3(0)))
            continue;  

        //ForcesApply
        if(glm::abs(collider->getVelocity().y) >0)
            collider->setIsGrounded(false);
        if(!collider->getIsGrounded())
            collider->applyForce(glm::vec3(0,-9.81,0));

        //Angular Momentum? TODO
        //Pivot at Center
        /*  float inertia = (1/12) * collider->getMass() * (glm::pow(collider->getBody().colliderScale.x*2,2)+glm::pow(collider->getBody().colliderScale.y*2,2) );
        //Torgue
        float torgueWidth = collider->getBody().colliderScale.x*glm::cos(glm::degrees(collider->getBody().colliderZRotation));
        float torgueHeight = collider->getBody().colliderScale.y*glm::cos(glm::degrees(collider->getBody().colliderZRotation));
        float torgue = (torgueWidth+torgueHeight);//*-9.81f*collider->getMass();

        float angularMomentum = 0;
        if(inertia!=0)
            angularMomentum = (torgue/inertia)*getTimeStep();
        if(collider->getNameOfEntityThisIsAttachedTo() == "Player")
            std::cout << angularMomentum <<"Torgue: " << torgue << " Inertia " << inertia << "\n";
        collider->setRot(collider->getRot()+angularMomentum); */
        
        
        //Friction ? NormalForce
        //Integrate
        collider->setVelocity(collider->getVelocity()+(collider->getAcceleration()*getTimeStep()));
        collider->setPos(collider->getPos()+(collider->getVelocity()*getTimeStep()));
        collider->setAcceleration(glm::vec3(0));
        collider->update(); //Nessecarry for corner pos updates?

        if(glm::abs(collider->getVelocity()) != glm::vec3(0))
            addColliderIntoHashTable(collider); 
    }
        
}

void PhysicsEngine::broadCollisionGathering()
{
    // Implemented using a hash table with morton encoding
    for (auto& [key, val] : mortonHashTable)
    {
        bool dontAddIfAllStatic = true;
        bool dontAddIfAllResting = true;
        if(val.size() > 1)
        {
            for(auto &entry:val)
                if(!entry->getIsStatic()||entry->getIsStatic()&&(entry->getVelocity()!=glm::vec3(0)))
                {
                    dontAddIfAllStatic = false;
                    break;
                }
            for(auto &entry:val)
                if(!entry->getIsResting())
                {
                    dontAddIfAllResting = false;
                    break;
                }            
        }
        if(!dontAddIfAllStatic&&!dontAddIfAllResting)
        {
            collisionsToResolve.push_back(val);
            continue;
        }           
            
    }
}

void PhysicsEngine::narrowCollisionGathering()
{
    // Implement through SAT collision detection
    std::vector<std::vector<PhysicsCollider*>> actualCollisions;

    for (auto& collisionList : collisionsToResolve)
    {
        std::vector<PhysicsCollider*> actualCollisionList;
        for (size_t i = 0; i < collisionList.size(); ++i)
        {
            for (size_t j = i + 1; j < collisionList.size(); ++j)
            {
                if (CollisionTester::arePhysicsCollidersColliding(collisionList[i], collisionList[j]))
                {
                    if (std::find(actualCollisionList.begin(), actualCollisionList.end(), collisionList[i]) == actualCollisionList.end())
                    {
                        actualCollisionList.push_back(collisionList[i]);
                    }
                    if (std::find(actualCollisionList.begin(), actualCollisionList.end(), collisionList[j]) == actualCollisionList.end())
                    {
                        actualCollisionList.push_back(collisionList[j]);
                    }
                }
            }
        }
        if (!actualCollisionList.empty())
        {
            actualCollisions.push_back(actualCollisionList);
        }
    }

    collisionsToResolve = actualCollisions;
}

void PhysicsEngine::collisionDetection()
{
    broadCollisionGathering();
    narrowCollisionGathering();
    maxCollisionsResolvedLastTick = collisionsToResolve.size();
}

void PhysicsEngine::collisionRespone()
{
    //Collision Response
    for(auto &listOfCollisionEntry:collisionsToResolve)
    {
        for(auto &entry:listOfCollisionEntry)
        {
            for(auto &collisiontoCheckFor:listOfCollisionEntry)
            {
                if(entry == collisiontoCheckFor)
                    continue;
                restoreInitialPosAndRot(entry);
                if((abs(entry->getVelocity().x) > 0.1f) || (abs(entry->getVelocity().y) > 0.1f))
                {
                    //Poor mans attempt to trade forces
                    glm::vec3 velocityToTrade(-entry->getVelocity()*collisiontoCheckFor->getElascicity());
                    entry->setVelocity(velocityToTrade);
                    if(!collisiontoCheckFor->getIsStatic())
                        collisiontoCheckFor->setVelocity(-velocityToTrade);
                }
                else
                {
                    entry->setVelocity(glm::vec3(0));
                    entry->setIsGrounded(true);
                } 
                           
            }
        }
    }
    collisionsToResolve.clear();
}

void PhysicsEngine::addColliderIntoHashTable(PhysicsCollider* colliderRef)
{
    if(isHalting)
        return;
    tableLogic.addColliderIntoHashTable(colliderRef, mortonHashTable);
}

void PhysicsEngine::removeColliderFromHashTable(PhysicsCollider *colliderRef)
{
    tableLogic.removeColliderFromHashTable(colliderRef, mortonHashTable);
}

int PhysicsEngine::getHashTableIndicesSize()
{
    int i = 0;
    for (auto& [key, val] : mortonHashTable)
    {
        if(val.size() != 0)
         i++;
    }
    return i;
}

void PhysicsEngine::getInitialTransform(float _deltatime)
{
    deltatime += _deltatime;
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
    ticksLastFrame = 0;
    
    if(isHalting) //Kinda just works if physicsObjects are not moving
        return;
    
    //Nessecary for Right HashTable Insertion because Corner Positions are getting calculated to late
    if(!initDone)
    {
        for(auto& entry:physicsObjects)
        {    
            addColliderIntoHashTable(entry);
            tableLogic.addColliderIntoHashTable(entry, mortonHashTable);
        }
        initDone = true;
    } 
    
    while (tickTime >= getTimeStep())
    {
        applyForces();
        collisionDetection();
        testing();
        /* for(auto &entry:collisionsToResolve)
        {
            std::cout << "COLLISION: \n";
            for(auto& val:entry)
                std::cout << val->getNameOfEntityThisIsAttachedTo() << "\n";
        } */
        collisionRespone();
        tickTime -= (1/tickrateOfSimulation);
        ticksLastFrame++;
    }
    //TicksPerSecondCalc
    if(deltatime < 1.0f)
        ticksBuffer+=ticksLastFrame;
    else
    {
        deltatime = 0;
        ticksCalculatedInOneSecond = ticksBuffer;
        ticksBuffer = 0;
    }
    //AfterTickTime For CollisionGathering
    for(auto entry:physicsObjects)
        if(entry->getVelocity() == glm::vec3(0))
            entry->setIsResting(true);
        else
            entry->setIsResting(false);

    initTransformOfColliders.clear();

}

//Helper Functions meant To Use Outside
bool PhysicsEngine::checkIfShellWouldCollide(glm::vec3 &pos, glm::vec3 &scale, float &rotZ)
{
    for(auto &entry:physicsObjects)
        if(CollisionTester::isColliderCollidingWithShell(entry, pos, scale, rotZ))
            return true;
    return false;
}

PhysicsCollider *PhysicsEngine::getFirstColliderShellCollidesWith(glm::vec3 &pos, glm::vec3 &scale, float &rotZ)
{
    for(auto &entry:physicsObjects)
        if(CollisionTester::isColliderCollidingWithShell(entry, pos, scale, rotZ))
            return entry;
    return nullptr;
}
