#include "CollisionTester.h"

std::vector<glm::vec2> CollisionTester::calcPointsWithRespectToRotation(PhysicsCollider* entity)
{
    //Order of Points: LeftBottom, RightTop, LeftTop, RightBottom
    std::vector<glm::vec2> calculatedPoints;

    float xScale = entity->getBody().colliderScale.x*glm::cos(glm::radians(entity->getBody().colliderZRotation)) - entity->getBody().colliderScale.y*glm::sin(glm::radians(entity->getBody().colliderZRotation));
    float yScale = entity->getBody().colliderScale.x*glm::sin(glm::radians(entity->getBody().colliderZRotation)) + entity->getBody().colliderScale.y*glm::cos(glm::radians(entity->getBody().colliderZRotation));
    glm::vec2 pointLeftBottom(entity->getBody().colliderPosition.x-(xScale), entity->getBody().colliderPosition.y-(yScale));
    glm::vec2 pointRightTop(entity->getBody().colliderPosition.x+xScale, entity->getBody().colliderPosition.y+yScale);

    xScale = entity->getBody().colliderScale.x*glm::cos(glm::radians(entity->getBody().colliderZRotation)*-1) - entity->getBody().colliderScale.y*glm::sin(glm::radians(entity->getBody().colliderZRotation)*-1);
    yScale = entity->getBody().colliderScale.x*glm::sin(glm::radians(entity->getBody().colliderZRotation)*-1) + entity->getBody().colliderScale.y*glm::cos(glm::radians(entity->getBody().colliderZRotation)*-1);
    
    glm::vec2 pointLeftTop(entity->getBody().colliderPosition.x-xScale, entity->getBody().colliderPosition.y+yScale);
    glm::vec2 pointRightBottom(entity->getBody().colliderPosition.x+xScale, entity->getBody().colliderPosition.y-yScale);

    calculatedPoints.push_back(pointLeftBottom);
    calculatedPoints.push_back(pointRightTop);
    calculatedPoints.push_back(pointLeftTop);
    calculatedPoints.push_back(pointRightBottom);

    return calculatedPoints;
}

float CollisionTester::signedDistancePointAnd2DBox(glm::vec3 pointPos, PhysicsCollider *colliderToCheck)
{
    glm::mat3 rotationMatrix = glm::toMat3(glm::angleAxis(glm::radians(colliderToCheck->getRot()), glm::vec3(0,0,1)));
    glm::vec3 localPoint = glm::transpose(rotationMatrix)*(pointPos - colliderToCheck->getPos());
    glm::vec3 q = glm::abs(localPoint) - colliderToCheck->getBody().colliderScale;
    glm::vec3 clampedQ = glm::max(q, glm::vec3(0.0f));
    float outsideDistance = glm::length(clampedQ);
    float insideDistance = glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);

    return outsideDistance+insideDistance;
}

float CollisionTester::signedDistance2DBoxAnd2DBox(PhysicsCollider *colliderToCheckOne, PhysicsCollider *colliderToCheckTwo)
{
    glm::mat3 rotationMatrixOne = glm::toMat3(glm::angleAxis(glm::radians(colliderToCheckOne->getRot()), glm::vec3(0,0,1)));
    glm::mat3 rotationMatrixTwo = glm::toMat3(glm::angleAxis(glm::radians(colliderToCheckTwo->getRot()), glm::vec3(0,0,1)));
    glm::vec3 pointOne = glm::transpose(rotationMatrixOne)* colliderToCheckOne->getPos();
    glm::vec3 pointTwo = glm::transpose(rotationMatrixOne)* colliderToCheckTwo->getPos();
    
    glm::vec3 localPoint = (pointOne - pointTwo);
    glm::vec3 q = glm::abs(localPoint) - colliderToCheckOne->getBody().colliderScale - colliderToCheckTwo->getBody().colliderScale;
    glm::vec3 clampedQ = glm::max(q, glm::vec3(0.0f));
    float outsideDistance = glm::length(clampedQ);
    float insideDistance = glm::min(glm::max(q.x, glm::max(q.y, q.z)), 0.0f);

    return outsideDistance+insideDistance;
}

std::vector<glm::vec2> CollisionTester::calcProjectionFieldOutOfPoints(std::vector<glm::vec2> pointsToUse)
{
    std::vector<glm::vec2> calculatedProjectionFields;
    glm::vec2 directionVectorLeftBottomLeftTop(pointsToUse[0]-pointsToUse[2]);
    glm::vec2 directionVectorLeftTopRightTop(pointsToUse[2] - pointsToUse[1]);
    glm::vec2 directionVectorRightTopRightBottom(pointsToUse[1] - pointsToUse[3]);
    glm::vec2 directionVectorRightBottomLeftBottom(pointsToUse[3] - pointsToUse[0]);

    glm::vec2 projectionField1(-directionVectorLeftBottomLeftTop.y, directionVectorLeftBottomLeftTop.x);
    glm::vec2 projectionField2(-directionVectorLeftTopRightTop.y, directionVectorLeftTopRightTop.x);
    glm::vec2 projectionField3(-directionVectorRightTopRightBottom.y, directionVectorRightTopRightBottom.x);
    glm::vec2 projectionField4(-directionVectorRightBottomLeftBottom.y, directionVectorRightBottomLeftBottom.x);

    calculatedProjectionFields.push_back(projectionField1);
    calculatedProjectionFields.push_back(projectionField2);
    calculatedProjectionFields.push_back(projectionField3);
    calculatedProjectionFields.push_back(projectionField4);

    return calculatedProjectionFields;
}

bool CollisionTester::calcIfProjectionFieldIsOverlapping(glm::vec2 projectionField, std::vector<glm::vec2> pointsE1, std::vector<glm::vec2> pointsE2)
{
    //Order of Points: LeftBottom, RightTop, LeftTop, RightBottom
    float leftBottomE1 = pointsE1[0].x*projectionField.x + pointsE1[0].y*projectionField.y;
    float leftBottomE2 = pointsE2[0].x*projectionField.x + pointsE2[0].y*projectionField.y;

    float rightTopE1 = pointsE1[1].x*projectionField.x + pointsE1[1].y*projectionField.y;
    float rightTopE2 = pointsE2[1].x*projectionField.x + pointsE2[1].y*projectionField.y;

    float leftTopE1 = pointsE1[2].x*projectionField.x + pointsE1[2].y*projectionField.y;
    float leftTopE2 = pointsE2[2].x*projectionField.x + pointsE2[2].y*projectionField.y;

    float rightBottomE1 = pointsE1[3].x*projectionField.x + pointsE1[3].y*projectionField.y;
    float rightBottomE2 = pointsE2[3].x*projectionField.x + pointsE2[3].y*projectionField.y;

    std::array<float, 4> e1 = {leftBottomE1, rightTopE1, leftTopE1, rightBottomE1};
    std::sort(e1.begin(), e1.end());
    std::array<float, 4> e2 = {leftBottomE2, rightTopE2, leftTopE2, rightBottomE2};
    std::sort(e2.begin(), e2.end());

    // e1[2,5] e2[1,6]
    if(e1[0]<=e2[0])
    {
        if(e2[0]<=e1[3])
        {
            return true;
        }
    }        
    else if(e1[0]<=e2[3])
    {
        return true;
    }
            
    return false;
}

bool CollisionTester::arePhysicsCollidersColliding(PhysicsCollider* e1, PhysicsCollider* e2)
{
    if(!simpleCheckForCollision(e1, e2))
        return false;
    //
    std::vector<glm::vec2> e1EdgePoints = calcPointsWithRespectToRotation(e1);
    std::vector<glm::vec2> e2EdgePoints = calcPointsWithRespectToRotation(e2);

    std::vector<glm::vec2> projFieldForEntity1 = calcProjectionFieldOutOfPoints(e1EdgePoints);
    std::vector<glm::vec2> projFieldForEntity2 = calcProjectionFieldOutOfPoints(e2EdgePoints);

    for(glm::vec2 projField : projFieldForEntity1)
    {
        if(!calcIfProjectionFieldIsOverlapping(projField, e1EdgePoints, e2EdgePoints))
            return false;
    }
    
    for(glm::vec2 projField : projFieldForEntity2)
    {
        if(!calcIfProjectionFieldIsOverlapping(projField, e1EdgePoints, e2EdgePoints))
            return false;
    }
    //std::cout << e1->getNameOfEntityThisIsAttachedTo() << " AND " << e2->getNameOfEntityThisIsAttachedTo() << " \n";
    return true;
}

bool CollisionTester::simpleCheckForCollision(PhysicsCollider* e1, PhysicsCollider* e2)
{
    float widthBoundingBoxE1, widthBoundingBoxE2;
    float gapXbetweeenE1E2, gapYbetweeenE1E2;
    
    //BoundingBox *1.2f because on unevenly Scales Boxes it sometimes is an Situation between perf/Precision
    if(e1->getBody().colliderScale.x >= e1->getBody().colliderScale.y)
        widthBoundingBoxE1 = e1->getBody().colliderScale.x*1.2f; // 1.2
    else 
        widthBoundingBoxE1 = e1->getBody().colliderScale.y*1.2f;
    
    if(e2->getBody().colliderScale.x >= e2->getBody().colliderScale.y)
        widthBoundingBoxE2 = e2->getBody().colliderScale.x*1.2f;
    else
        widthBoundingBoxE2 = e2->getBody().colliderScale.y*1.2f;

    //X-GAP based on Position
    if(e1->getBody().colliderPosition.x <= e2->getBody().colliderPosition.x)
        gapXbetweeenE1E2 = (e2->getBody().colliderPosition.x - widthBoundingBoxE2) - (e1->getBody().colliderPosition.x + widthBoundingBoxE1);
    else 
        gapXbetweeenE1E2 = (e1->getBody().colliderPosition.x - widthBoundingBoxE1) - (e2->getBody().colliderPosition.x + widthBoundingBoxE2);
    
    //Y-GAP based on Position
    if(e1->getBody().colliderPosition.y <= e2->getBody().colliderPosition.y)
        gapYbetweeenE1E2 = (e2->getBody().colliderPosition.y - widthBoundingBoxE2) - (e1->getBody().colliderPosition.y + widthBoundingBoxE1);
    else 
        gapYbetweeenE1E2 = (e1->getBody().colliderPosition.y - widthBoundingBoxE1) - (e2->getBody().colliderPosition.y + widthBoundingBoxE2);

    //std::cout << "gapX: " << gapXbetweeenE1E2 << "\n";
    //std::cout << "gapY: " << gapYbetweeenE1E2 << "\n";
    if((gapXbetweeenE1E2 <= 0) && (gapYbetweeenE1E2 <= 0))
        return true;
    return false;
}

bool CollisionTester::simpleCollisionCheckBasedOnDistanceAndScale(PhysicsCollider *e1, PhysicsCollider *e2)
{
    float xGapBetween = abs(e1->getBody().colliderPosition.x - e2->getBody().colliderPosition.x);
    xGapBetween = xGapBetween - e1->getBody().colliderScale.x - e2->getBody().colliderScale.x;

    float yGapBetween = abs(e1->getBody().colliderPosition.y - e2->getBody().colliderPosition.y);
    yGapBetween = yGapBetween - e1->getBody().colliderScale.y - e2->getBody().colliderScale.y;

    if(xGapBetween <= 0 && yGapBetween <= 0)
        return true;
    return false;
}

bool CollisionTester::aAABCollisionCheck(PhysicsCollider *e1, PhysicsCollider *e2)
{
    return (e1->getPos().x < e2->getPos().x + e2->getBody().colliderScale.x &&
            e1->getPos().x + e1->getBody().colliderScale.x > e2->getPos().x &&
            e1->getPos().y < e2->getPos().y + e2->getBody().colliderScale.y &&
            e1->getPos().y + e1->getBody().colliderScale.y > e2->getPos().y);;
}
