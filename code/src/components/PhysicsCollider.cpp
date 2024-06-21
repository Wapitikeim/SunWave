#include "PhysicsCollider.h"

void PhysicsCollider::updateEntityPosAndRot()
{
    entityThisIsAttachedTo->setPosition(colliderBody.colliderPosition);
    entityThisIsAttachedTo->setZRotation(colliderBody.colliderZRotation);
}

void PhysicsCollider::update()
{
    colliderBody.colliderPosition = entityThisIsAttachedTo->getPosition();
    colliderBody.colliderScale = entityThisIsAttachedTo->getScale();
    colliderBody.colliderZRotation = entityThisIsAttachedTo->getRotation();
}