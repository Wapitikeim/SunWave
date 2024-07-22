#include "PhysicsCollider.h"

void PhysicsCollider::updateEntityPosAndRot()
{
    entityThisIsAttachedTo->setPosition(colliderBody.colliderPosition);
    entityThisIsAttachedTo->setZRotation(colliderBody.colliderZRotation);
}

void PhysicsCollider::updateCornerPositions()
{
    float xScale = colliderBody.colliderScale.x*glm::cos(glm::radians(colliderBody.colliderZRotation)) - colliderBody.colliderScale.y*glm::sin(glm::radians(colliderBody.colliderZRotation));
    float yScale = colliderBody.colliderScale.x*glm::sin(glm::radians(colliderBody.colliderZRotation)) + colliderBody.colliderScale.y*glm::cos(glm::radians(colliderBody.colliderZRotation));
    
    cornerPos.leftBottom = glm::vec3(
        colliderBody.colliderPosition.x-xScale,
        colliderBody.colliderPosition.y-yScale,
        colliderBody.colliderPosition.z);
    cornerPos.rightTop = glm::vec3(
        colliderBody.colliderPosition.x+xScale,
        colliderBody.colliderPosition.y+yScale,
        colliderBody.colliderPosition.z);
    
    xScale = colliderBody.colliderScale.x*glm::cos(glm::radians(colliderBody.colliderZRotation)*-1) - colliderBody.colliderScale.y*glm::sin(glm::radians(colliderBody.colliderZRotation)*-1);
    yScale = colliderBody.colliderScale.x*glm::sin(glm::radians(colliderBody.colliderZRotation)*-1) + colliderBody.colliderScale.y*glm::cos(glm::radians(colliderBody.colliderZRotation)*-1);

    cornerPos.leftTop = glm::vec3(
        colliderBody.colliderPosition.x-xScale,
        colliderBody.colliderPosition.y+yScale,
        colliderBody.colliderPosition.z);
    cornerPos.rightBottom = glm::vec3(
        colliderBody.colliderPosition.x+xScale,
        colliderBody.colliderPosition.y-yScale,
        colliderBody.colliderPosition.z);
    
    //vecUpdate
    cornerPos.cornerVec.clear();
    cornerPos.cornerVec = {cornerPos.leftBottom, cornerPos.leftTop, cornerPos.rightTop, cornerPos.rightBottom};
}

void PhysicsCollider::update()
{
    colliderBody.colliderPosition = entityThisIsAttachedTo->getPosition();
    colliderBody.colliderScale = entityThisIsAttachedTo->getScale();
    colliderBody.colliderZRotation = entityThisIsAttachedTo->getRotation();

    //Exclude This update For Static Entities except if they are moving
    if(!isStatic || colliderBody.colliderVelocity != glm::vec3(0))
        updateCornerPositions();
}