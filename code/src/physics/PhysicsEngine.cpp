#include "PhysicsEngine.h"

void PhysicsEngine::restoreInitialPosAndRot(PhysicsCollider* collider)
{
    for(auto entry:initTransformOfColliders)
        {
            if(entry.ref == collider)
            {
                collider->setPos(entry.colliderInitialPos);
                collider->setRot(entry.colliderInitialRot);
            }
        }
    
    //collider.setPos(colliderInitialPos);
    //collider.setRot(colliderInitialRot);
}

void PhysicsEngine::addColliderIntoHashTable(PhysicsCollider* colliderRef)
{
    removeColliderFromHashTable(colliderRef);
    colliderRef->clearIndiciesForHashTableOfThisEntity();
    
    int tableSize = glm::ceil((cameraXHalf*2*cameraYHalf*2)/spacing);
    if(colliderRef->getBody().colliderScale.x > (spacing/2) || colliderRef->getBody().colliderScale.y > (spacing/2))
    {
        int segmentsX = glm::floor(colliderRef->getBody().colliderScale.x);
        int segmentsY = glm::floor(colliderRef->getBody().colliderScale.y);
        int segments = glm::max(segmentsX,segmentsY);
        
        glm::vec3 leftToRightBottom = colliderRef->getCornerPos().rightBottom - colliderRef->getCornerPos().leftBottom;
        float deltaXBottom = leftToRightBottom.x / segments;
        float deltaYBottom = leftToRightBottom.y / segments;

        glm::vec3 rightToRight = colliderRef->getCornerPos().rightBottom - colliderRef->getCornerPos().rightTop;
        float deltaXRight = rightToRight.x / segments;
        float deltaYRight = rightToRight.y / segments;

        glm::vec3 leftToLeft = colliderRef->getCornerPos().leftBottom - colliderRef->getCornerPos().leftTop;
        float deltaXLeft = leftToLeft.x / segments;
        float deltaYLeft = leftToLeft.y / segments;

        glm::vec3 leftToRightTop = colliderRef->getCornerPos().rightTop - colliderRef->getCornerPos().leftTop;
        float deltaXTop = leftToRightTop.x / segments;
        float deltaYTop = leftToRightTop.y / segments;
        //std:: cout << segments << "\n";
        for(int i = 1; i < segments ; i++)
        {
            float newPointX = colliderRef->getCornerPos().leftBottom.x + deltaXBottom*i;
            float newPointY = colliderRef->getCornerPos().leftBottom.y + deltaYBottom*i;
            pointsToGetIndexesFor.push_back(glm::vec2(glm::floor(newPointX/spacing), glm::floor(newPointY/spacing)));
            
            newPointX = colliderRef->getCornerPos().rightTop.x + deltaXRight*i;
            newPointY = colliderRef->getCornerPos().rightTop.y + deltaYRight*i;
            pointsToGetIndexesFor.push_back(glm::vec2(glm::floor(newPointX/spacing), glm::floor(newPointY/spacing)));
            
            newPointX = colliderRef->getCornerPos().leftTop.x + deltaXTop*i;
            newPointY = colliderRef->getCornerPos().leftTop.y + deltaYTop*i;
            pointsToGetIndexesFor.push_back(glm::vec2(glm::floor(newPointX/spacing), glm::floor(newPointY/spacing)));

            newPointX = colliderRef->getCornerPos().leftTop.x + deltaXLeft*i;
            newPointY = colliderRef->getCornerPos().leftTop.y + deltaYLeft*i;
            pointsToGetIndexesFor.push_back(glm::vec2(glm::floor(newPointX/spacing), glm::floor(newPointY/spacing)));
            
        }
    }
        
    glm::vec2 cornerPointX(glm::floor((colliderRef->getCornerPos().leftBottom.x) /spacing), glm::floor((colliderRef->getCornerPos().leftBottom.y) /spacing));
    pointsToGetIndexesFor.push_back(cornerPointX);
    cornerPointX = glm::vec2(glm::floor((colliderRef->getCornerPos().leftTop.x) /spacing), glm::floor((colliderRef->getCornerPos().leftTop.y) /spacing));
    pointsToGetIndexesFor.push_back(cornerPointX);
    cornerPointX = glm::vec2(glm::floor((colliderRef->getCornerPos().rightTop.x) /spacing), glm::floor((colliderRef->getCornerPos().rightTop.y) /spacing));
    pointsToGetIndexesFor.push_back(cornerPointX);
    cornerPointX = glm::vec2(glm::floor((colliderRef->getCornerPos().rightBottom.x) /spacing), glm::floor((colliderRef->getCornerPos().rightBottom.y) /spacing));
    pointsToGetIndexesFor.push_back(cornerPointX);

    for(auto &entry:pointsToGetIndexesFor)
    {
        int index = entry.y* glm::ceil((cameraXHalf*2)/(spacing-1))  + entry.x;
        index = index;//%tableSize;
        indiciesForHashTable.push_back(index); 
    }
    std::sort(indiciesForHashTable.begin(), indiciesForHashTable.end());
    indiciesForHashTable.erase(std::unique(indiciesForHashTable.begin(),indiciesForHashTable.end()), indiciesForHashTable.end());
    
    for(auto &entry:indiciesForHashTable)
    {
        hashTable[entry].push_back(colliderRef);
        colliderRef->addOneIndexIntoIndiciesForHashTable(entry);
    } 
    indiciesForHashTable.clear();
    pointsToGetIndexesFor.clear();
}

void PhysicsEngine::removeColliderFromHashTable(PhysicsCollider *colliderRef)
{
    for (auto& [key, val] : hashTable)
    {
        val.erase(std::remove(val.begin(), val.end(), colliderRef),val.end());
    }
}

void PhysicsEngine::addColliderNeighboursAlso(PhysicsCollider *colliderRef)
{
    for(auto &entry:colliderRef->getTableIndicies())
    {
        hashTable[entry+1].push_back(colliderRef);
        hashTable[entry-1].push_back(colliderRef);
        hashTable[entry+glm::ceil((cameraXHalf*2)/(spacing-1))].push_back(colliderRef);
        hashTable[entry-glm::ceil((cameraXHalf*2)/(spacing-1))].push_back(colliderRef);
    }
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
    if(isHalting) //Kinda just works if physicsObjects are not moving
        return;
    
    //Nessecary for Right HashTable Insertion because Corner Positions are getting calculated to late
    if(!initDone)
    {
        for(auto& entry:physicsObjects)
        {    
            addColliderIntoHashTable(entry);
        }
        initDone = true;
    } 
    
    while (tickTime >= getTimeStep())
    {
        //Force Apply + Update
        for(auto& collider:physicsObjects)
        {
            if(collider->getIsTrigger() || collider->getIsStatic()&&(collider->getVelocity()==glm::vec3(0)))
                continue;
            /* if(!(abs(collider->getVelocity().x) > 0.1f) || !(abs(collider->getVelocity().y) > 0.1f))
            {
                addColliderIntoHashTable(collider);
                addColliderNeighboursAlso(collider);
            }  */   
            addColliderIntoHashTable(collider);
            //addColliderNeighboursAlso(collider);
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
            
            //Collision Test
            /* for(auto& colliderToCheckCollisionFor : physicsObjects)
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
              */
        }
        //"Smarter" Collision Test
        for (auto& [key, val] : hashTable)
        {
            bool dontAddIfAllStatic = true;
            if(val.size() > 1)
                for(auto &entry:val)
                    if(!entry->getIsStatic()||entry->getIsStatic()&&(entry->getVelocity()!=glm::vec3(0)))
                    {
                        dontAddIfAllStatic = false;
                        break;
                    }             
            if(!dontAddIfAllStatic)
                collisionsToResolve.push_back(val);
        }
        //Collision Response
        for(auto &listOfCollisionEntry:collisionsToResolve)
        {
            for(auto &entry:listOfCollisionEntry)
            {
                for(auto &collisiontoCheckFor:listOfCollisionEntry)
                {
                    if(entry == collisiontoCheckFor)
                        continue;
                    if(CollisionTester::arePhysicsCollidersColliding(entry, collisiontoCheckFor))
                    {
                        //Getting the Edge of the collision contact
                        //Get the 'Save' point
                        int index = 0;
                        float shortestDistance = 100;
                        int indexShortestDistance;
                        auto& refCornerVec = collisiontoCheckFor->getCornerPosAsVector(); 
                        //"Save" Corner Pos calc
                        for(auto &cornerToCheck:refCornerVec)
                        {
                            float distanceToCalculate = 0;
                            //Go Through all Corners of Entity
                            for(auto &cornerOfEntity:entry->getCornerPosAsVector())
                                distanceToCalculate+=glm::distance(cornerToCheck, cornerOfEntity);
                            
                            if(distanceToCalculate<shortestDistance)
                            {
                                shortestDistance = distanceToCalculate;
                                indexShortestDistance = index;
                            }   
                            index++;
                        }
                        //Possible Edge Index Calc (Maybe smarter/Better way)
                        int possibleEdgeIndexes1[2];possibleEdgeIndexes1[0] = indexShortestDistance;
                        int possibleEdgeIndexes2[2];possibleEdgeIndexes2[0] = indexShortestDistance;
                        if((indexShortestDistance+1)>3)
                            possibleEdgeIndexes1[1] = 0;
                        else
                            possibleEdgeIndexes1[1] = indexShortestDistance+1;
                        if((indexShortestDistance-1)<0)
                            possibleEdgeIndexes2[1] = 3;
                        else
                            possibleEdgeIndexes2[1] = indexShortestDistance-1;
                        //Which Edge is the correct one?
                        //Which possible edge is the shorter one?
                        int theShorterOne = 0;
                        
                        float dEdge1 = glm::distance(refCornerVec[possibleEdgeIndexes1[0]], refCornerVec[possibleEdgeIndexes1[1]]);
                        float dEdge2 = glm::distance(refCornerVec[possibleEdgeIndexes2[0]], refCornerVec[possibleEdgeIndexes2[1]]);
                        if(dEdge1>dEdge2)
                            theShorterOne = 2;
                        else
                            theShorterOne = 1;
                        
                        //Vector Calc
                        glm::vec3 edge1 = refCornerVec[possibleEdgeIndexes1[1]]-refCornerVec[possibleEdgeIndexes1[0]];
                        glm::vec3 edge2 = refCornerVec[possibleEdgeIndexes2[1]]-refCornerVec[possibleEdgeIndexes2[0]]; 
                        float scalingFactor = 0;
                        glm::vec3 pointThatDeterminesTheShorterEdge;
                        if(theShorterOne == 2)
                        {
                            scalingFactor = dEdge2/dEdge1;
                            pointThatDeterminesTheShorterEdge = refCornerVec[possibleEdgeIndexes1[0]] + scalingFactor * edge1;
                        }   
                        else
                        {
                            scalingFactor = dEdge1/dEdge2; 
                            pointThatDeterminesTheShorterEdge = refCornerVec[possibleEdgeIndexes2[0]] + scalingFactor * edge2;
                        }
                        float distanceToTheLongEdgePoint = 0;
                        float distanceToTheShortSidePoint = 0;
                        for(auto &cornerOfEntity:entry->getCornerPosAsVector())
                        {
                            distanceToTheLongEdgePoint+=glm::distance(pointThatDeterminesTheShorterEdge, cornerOfEntity);
                            if(theShorterOne == 2)
                                distanceToTheShortSidePoint+=glm::distance(refCornerVec[possibleEdgeIndexes2[1]], cornerOfEntity);
                            else
                                distanceToTheShortSidePoint+=glm::distance(refCornerVec[possibleEdgeIndexes1[1]], cornerOfEntity);
                        }
                        //Now we finally can determine which edge is the right one      
                        int longerEdge = 0;
                        if(glm::length(edge1)> glm::length(edge2))
                            longerEdge = 1;
                        else
                            longerEdge = 2;
                        glm::vec3 edgeToUseCalculationsWith;
                        if(distanceToTheLongEdgePoint < distanceToTheShortSidePoint)
                            if(longerEdge == 1)
                                edgeToUseCalculationsWith = edge1;
                            else
                                edgeToUseCalculationsWith = edge2;
                        else
                            if(longerEdge == 1)
                                edgeToUseCalculationsWith = edge2;
                            else
                                edgeToUseCalculationsWith = edge1;
                            
                        glm::vec2 normVector1(-edgeToUseCalculationsWith.y, edgeToUseCalculationsWith.x);
                        glm::vec2 normVector2(edgeToUseCalculationsWith.y, -edgeToUseCalculationsWith.x);

                        
                                                 
                        
                        restoreInitialPosAndRot(entry);
                        if(entry->getNameOfEntityThisIsAttachedTo() == "Player")
                        {
                            //std::cout << glm::degrees(glm::atan((normVector1/glm::length(normVector1)).x,(normVector1/glm::length(normVector1)).y)) << "\n";
                            //std::cout << glm::degrees(glm::atan((normVector2/glm::length(normVector2)).x,(normVector2/glm::length(normVector2)).y)) << "\n";
                            //std::cout << "\n";
                            entry->setRot(glm::degrees(glm::atan((normVector1/glm::length(normVector1)).x,(normVector1/glm::length(normVector1)).y)));
                        }
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
        }
        //std::cout << collisionsToResolve.size() << "\n";
        collisionsToResolve.clear();
        tickTime -= (1/tickrateOfSimulation);
    }
    initTransformOfColliders.clear();
}