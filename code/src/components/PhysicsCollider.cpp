#include "PhysicsCollider.h"

void PhysicsCollider::setBody(const PhysicsBody &newBody)
{
    colliderBody = newBody;
    entityThisIsAttachedTo->setPosition(newBody.colliderPosition);
    entityThisIsAttachedTo->setZRotation(newBody.colliderZRotation);
}

void PhysicsCollider::update()
{
    colliderBody.colliderPosition = entityThisIsAttachedTo->getPosition();
    colliderBody.colliderScale = entityThisIsAttachedTo->getScale();
    colliderBody.colliderZRotation = entityThisIsAttachedTo->getRotation();
}