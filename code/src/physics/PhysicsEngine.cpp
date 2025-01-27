#include "PhysicsEngine.h"
#include <bitset>
#include <unordered_set>
#include <utility>
#include <functional>

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator() (const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

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
/*     if (collisionsToResolve.size() == 0)
        return;
    
    //std::cout << "Collisions to Resolve:" << std::endl;
    for (const auto& collisionList : collisionsToResolve)
    {
        //std::cout << "Collision Group:" << std::endl;
        for (const auto& collider : collisionList)
        {
            //std::cout << "  Collider: " << collider->getNameOfEntityThisIsAttachedTo() << " ";
            // Change color to red if colliding
            if(collider!=nullptr)
                collider->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange", glm::vec4(1, 0, 0, 1));
        }
    }   

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
            if(collider!=nullptr)
                collider->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange", glm::vec4(0, 0, 0, 1));
        }
    }
 */
    if(physicsObjects[0]->getNameOfEntityThisIsAttachedTo() == "Player")
        if(physicsObjects[0]->getIsInContact())
            physicsObjects[0]->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange", glm::vec4(1, 0, 0, 1));
        else
            physicsObjects[0]->getEntityThisIsAttachedTo()->getShaderContainer().setUniformVec4("colorChange", glm::vec4(0, 0, 0, 1));
}

void PhysicsEngine::updatePhysicsState()
{
    for(auto& collider:physicsObjects)
    {
        if(collider->getIsTrigger() || collider->getIsStatic()&&(collider->getVelocity()==glm::vec3(0)))
            continue;  

        applyGravity(collider);
        integrateForces(collider);
        if(glm::abs(collider->getVelocity()) != glm::vec3(0))
            addColliderIntoHashTable(collider); 
    }       
}

void PhysicsEngine::applyGravity(PhysicsCollider *collider)
{
    //If the collider has some y velocity > Threshhold and wasnt part of a collision he cant be grounded
    if(glm::abs(collider->getVelocity().y) < RESTING_THRESHOLD)
        collider->setIsGrounded(true);
    else
        collider->setIsGrounded(false);
    
    if(collider->getIsGrounded() && !collider->getIsInContact())
        collider->setIsGrounded(false);

    if(!collider->getIsGrounded() && !collider->getUnaffectedByGravity())
    {
        glm::vec3 gravityForce = glm::vec3(0, GRAVITY, 0) * collider->getMass();
        collider->applyForce(gravityForce);
    }
    
}

void PhysicsEngine::integrateForces(PhysicsCollider *collider)
{
    // Integrate acceleration to update velocity
    collider->setVelocity(collider->getVelocity() + collider->getAcceleration() * getTimeStep());

    // Apply air resistance (reduce velocity by 0.5%)
    collider->setVelocity(collider->getVelocity() * AIR_RESISTANCE);
    if(glm::abs(collider->getVelocity().x) < RESTING_THRESHOLD)
        collider->setVelocity(glm::vec3(0, collider->getVelocity().y, collider->getVelocity().z));

    if(collider->getLockX())
        collider->setVelocity(glm::vec3(0,collider->getVelocity().y, collider->getVelocity().z));
    if(collider->getLockY())
        collider->setVelocity(glm::vec3(collider->getVelocity().x,0, collider->getVelocity().z));
    // Integrate velocity to update position
    collider->setPos(collider->getPos() + collider->getVelocity() * getTimeStep());

    // Clear accumulated forces after integration
    collider->clearForces();

    if(collider->getIsResting())
        collider->setAcceleration(glm::vec3(0));
        
    collider->update();
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

void PhysicsEngine::updateNonCollidingColliders()
{
    //Potentially expensive and not needed?
    for (auto& collider : physicsObjects)
    {
        if(collider->getIsTrigger() || collider->getIsStatic() || collider->getIsResting() ||collider->getIsTrigger())
            continue;
        bool isColliding = false;
        for (const auto& collisionList : collisionsToResolve)
        {
            if (std::find(collisionList.begin(), collisionList.end(), collider) != collisionList.end())
            {
                isColliding = true;
                break;
            }
        }
        if (!isColliding&&collider->getIsGrounded())
            collider->setIsInContact(false);
        if(!isColliding&&collider->getVelocity()!=glm::vec3(0))
            collider->setIsInContact(false);
            
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
    updateNonCollidingColliders();
    maxCollisionsResolvedLastTick = collisionsToResolve.size();
}

void PhysicsEngine::collisionRespone()
{
    // Collision Response
    std::unordered_set<std::pair<PhysicsCollider*, PhysicsCollider*>, pair_hash> processedCollisions;    
    for (auto& listOfCollisionEntry : collisionsToResolve)
    {
        for (size_t i = 0; i < listOfCollisionEntry.size(); ++i)
        {
            for (size_t j = i + 1; j < listOfCollisionEntry.size(); ++j)
            {
                PhysicsCollider* cA = listOfCollisionEntry[i];
                PhysicsCollider* cB = listOfCollisionEntry[j];
                // Skip response if both colliders are static or resting
                if (cA->getIsStatic() && cB->getIsStatic() || cA->getIsResting() && cB->getIsResting())
                    continue;
                
                if(cA->getIsTrigger()||cB->getIsTrigger())
                    continue;
                
                // Check if this collision has already been processed
                if (processedCollisions.find(std::make_pair(cA, cB)) != processedCollisions.end() ||
                    processedCollisions.find(std::make_pair(cB, cA)) != processedCollisions.end())
                    continue;

                // Calculate relative velocity
                glm::vec3 relativeVelocity = cB->getVelocity() - cA->getVelocity();
                glm::vec3 contactNormal;
                float penetrationDepth;
                if (CollisionTester::arePhysicsCollidersCollidingWithDetails(cA, cB, contactNormal, penetrationDepth))
                {
                    if(glm::abs(relativeVelocity.x) > RESTING_THRESHOLD|| glm::abs(relativeVelocity.y) > RESTING_THRESHOLD)
                        resolveCollision(cA, cB, contactNormal, penetrationDepth, relativeVelocity);
                    else if(contactNormal.y > 0.99) //Small arc of 5.7 degrees still counts as ground
                    {
                        //Collision potentially very weak 
                        cA->setVelocity(glm::vec3(cA->getVelocity().x, 0, cA->getVelocity().z));
                        cA->setAcceleration(glm::vec3(cA->getAcceleration().x, 0, cA->getAcceleration().z));
                        cA->setIsInContact(true);
                        cB->setVelocity(glm::vec3(cB->getVelocity().x, 0, cB->getVelocity().z));
                        cB->setAcceleration(glm::vec3(cB->getAcceleration().x, 0, cB->getAcceleration().z));
                        cB->setIsInContact(true);
                    }
                    else if(glm::abs(contactNormal.x) == 1) //Colllision weak and onto wall
                    {
                        cA->setVelocity(glm::vec3(0, cA->getVelocity().y, cA->getVelocity().z));
                        cA->setAcceleration(glm::vec3(0, cA->getAcceleration().y, cA->getAcceleration().z));
                        cB->setVelocity(glm::vec3(0, cB->getVelocity().y, cB->getVelocity().z));
                        cB->setAcceleration(glm::vec3(0, cB->getAcceleration().y, cB->getAcceleration().z));
                    }
                    processedCollisions.insert(std::make_pair(cA, cB));
                }
                else
                {
                    //Actually only happens if listOfCollisionEntry.size() > 2
                    // -> makes sense because narrowCollisionGathering only counts collisions
                    //Not colliding but potentially touching
                    //Important for small Entities -> e.g. just 1 hash entry for all points
                    processedCollisions.insert(std::make_pair(cA, cB));
                    continue;
                }
            }
        }
    }
    collisionsToResolve.clear();
}

void PhysicsEngine::resolveCollision(PhysicsCollider* colliderA, PhysicsCollider* colliderB, const glm::vec3& contactNormal, float penetrationDepth, const glm::vec3& relativeVelocity)
{
    // Calculate relative velocity along the contact normal
    float velocityAlongNormal = glm::dot(relativeVelocity, contactNormal);
    // Skip if colliders are moving apart
    if (velocityAlongNormal > 0)
        return;
    
    // Calculate restitution (elasticity)
    float restitution = std::min(colliderA->getElascicity(), colliderB->getElascicity());
    // Calculate impulse scalar
    float impulseScalar = -(1 + restitution) * velocityAlongNormal;
    impulseScalar /= (1.0f / colliderA->getMass()) + (1.0f / colliderB->getMass());
    if(impulseScalar < 0.1f)
        return;
    // Apply impulse to colliders
    glm::vec3 impulse = (impulseScalar*BOUNCE_MULTIPLIER) * contactNormal;
    //std::cout << "Impulse: " << impulse.x << " " << impulse.y << " " << impulse.z << "\n";
    if (!colliderA->getIsStatic())
        colliderA->setVelocity(colliderA->getVelocity() - impulse * (1.0f / colliderA->getMass()));

    if (!colliderB->getIsStatic())
        colliderB->setVelocity(colliderB->getVelocity() + impulse * (1.0f / colliderB->getMass()));
    
    // Debug prints for rotated colliders
    /* std::cout << "Contact Normal: " << contactNormal.x << ", " << contactNormal.y << ", " << contactNormal.z << "\n";
    std::cout << "Velocity Along Normal: " << velocityAlongNormal << "\n";
    std::cout << "Relative Velocity: " << relativeVelocity.x << ", " << relativeVelocity.y << ", " << relativeVelocity.z << "\n";
    std::cout << "Impulse: " << impulse.x << ", " << impulse.y << ", " << impulse.z << "\n";
    std::cout << "Penetration: " << penetrationDepth << "\n";
    std::cout << "\n"; */
    // Positional correction to avoid sinking
    /* float percent = 0.8f; // Slightly increased penetration correction percentage
    float slop = 0.01f; // Small bias to prevent sinking
    glm::vec3 correction = std::max(penetrationDepth - slop, 0.0f) / ((1.0f / colliderA->getMass()) + (1.0f / colliderB->getMass())) * percent * contactNormal;
    */
    if (!colliderA->getIsStatic())
    {
        restoreInitialPosAndRot(colliderA);
        //addColliderIntoHashTable(colliderA);
    }
        
    if (!colliderB->getIsStatic())
    {
        restoreInitialPosAndRot(colliderB);
        //addColliderIntoHashTable(colliderB);
    }
    
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
    if(mortonHashTable.size() == 0)
        return 0;
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

    if (isHalting || physicsObjects.empty())
        return;

    // Necessary for Right HashTable Insertion because Corner Positions are getting calculated too late
    if (!initDone)
    {
        for (auto& entry : physicsObjects)
        {
            addColliderIntoHashTable(entry);
            if(!entry->getIsStatic())
                entry->setVelocity(glm::vec3(0, RESTING_THRESHOLD, 0));
        }
            
        initDone = true;
    }

    while (tickTime >= getTimeStep())
    {
        updatePhysicsState();
        collisionDetection();
        testing();
        collisionRespone();
        tickTime -= getTimeStep();
        ticksLastFrame++;
    }

    // TicksPerSecondCalc
    if (deltatime < 1.0f)
        ticksBuffer += ticksLastFrame;
    else
    {
        deltatime = 0;
        ticksCalculatedInOneSecond = ticksBuffer;
        ticksBuffer = 0;
    }

    // AfterTickTime For CollisionGathering
    for (auto entry : physicsObjects)
        if (entry->getVelocity() == glm::vec3(0))
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

bool PhysicsEngine::checkTriggerColliderCollision(const std::string &colliderEntityName, const std::string &triggerColliderEntityName)
{
    PhysicsCollider* collider = nullptr;
    PhysicsCollider* triggerCollider = nullptr;

    // Search for the colliders in the physicsObjects vector
    for (auto& entry : physicsObjects)
    {
        if (entry->getEntityThisIsAttachedTo()->getEntityName() == colliderEntityName)
            collider = entry;
        if (entry->getEntityThisIsAttachedTo()->getEntityName() == triggerColliderEntityName)
            triggerCollider = entry;
    }

    if (!collider || !triggerCollider)
        return false;

    glm::vec3 contactNormal;
    float penetrationDepth;
    return CollisionTester::arePhysicsCollidersCollidingWithDetails(collider, triggerCollider, contactNormal, penetrationDepth);
}

bool PhysicsEngine::checkColliderPlayerCollision(const std::string &colliderEntityName)
{
    PhysicsCollider* collider = nullptr;
    PhysicsCollider* colliderTwo = nullptr;

    // Search for the colliders in the physicsObjects vector
    for (auto& entry : physicsObjects)
    {
        if (entry->getEntityThisIsAttachedTo()->getEntityName() == colliderEntityName)
            collider = entry;
        if (entry->getEntityThisIsAttachedTo()->getEntityName() == "Player")
            colliderTwo = entry;
    }

    if (!collider || !colliderTwo)
        return false;
    //Basically just here to inflate the scale for the catch shapes minigames
    colliderTwo->setScale(colliderTwo->getBody().colliderScale*1.2f);
    glm::vec3 contactNormal;
    float penetrationDepth;
    return CollisionTester::arePhysicsCollidersCollidingWithDetails(collider, colliderTwo, contactNormal, penetrationDepth);
}

PhysicsCollider *PhysicsEngine::getFirstColliderShellCollidesWith(glm::vec3 &pos, glm::vec3 &scale, float& rotZ)
{
    for(auto &entry:physicsObjects)
        if(CollisionTester::isColliderCollidingWithShell(entry, pos, scale, rotZ))
            return entry;
    return nullptr;
}
