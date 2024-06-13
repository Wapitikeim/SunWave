#include "CollisionTester.h"

std::vector<glm::vec2> CollisionTester::calcPointsWithRespectToRotation(Entity* entity)
{
    //Order of Points: LeftBottom, RightTop, LeftTop, RightBottom
    std::vector<glm::vec2> calculatedPoints;

    float xScale = entity->getScale().x*glm::cos(glm::radians(entity->getRotation())) - entity->getScale().y*glm::sin(glm::radians(entity->getRotation()));
    float yScale = entity->getScale().x*glm::sin(glm::radians(entity->getRotation())) + entity->getScale().y*glm::cos(glm::radians(entity->getRotation()));
    
    glm::vec2 pointLeftBottom(entity->getPosition().x-(xScale), entity->getPosition().y-(yScale));
    glm::vec2 pointRightTop(entity->getPosition().x+xScale, entity->getPosition().y+yScale);

    xScale = entity->getScale().x*glm::cos(glm::radians(entity->getRotation())*-1) - entity->getScale().y*glm::sin(glm::radians(entity->getRotation())*-1);
    yScale = entity->getScale().x*glm::sin(glm::radians(entity->getRotation())*-1) + entity->getScale().y*glm::cos(glm::radians(entity->getRotation())*-1);
    
    glm::vec2 pointLeftTop(entity->getPosition().x-xScale, entity->getPosition().y+yScale);
    glm::vec2 pointRightBottom(entity->getPosition().x+xScale, entity->getPosition().y-yScale);

    calculatedPoints.push_back(pointLeftBottom);
    calculatedPoints.push_back(pointRightTop);
    calculatedPoints.push_back(pointLeftTop);
    calculatedPoints.push_back(pointRightBottom);

    return calculatedPoints;
}

float CollisionTester::signedDistance2DBox(glm::vec3 posToCheckTo, glm::vec3 posScale, glm::vec3 objectScale, glm::vec3 objectPos, float rotation)
{
    float angle = rotation *3.14159 * 2 * -1;
    objectPos.x = (objectPos.x * glm::cos(angle)) - (objectPos.y * glm::sin(angle));
    objectPos.y = (objectPos.x * glm::sin(angle)) + (objectPos.y * glm::cos(angle));
    //Translation
    glm::vec2 position = glm::vec2(posToCheckTo.x, posToCheckTo.y)-glm::vec2(objectPos.x, objectPos.y) + glm::vec2(posScale.x/2, posScale.y/2);
    
    glm::vec2 distance = glm::abs(position) - glm::vec2(objectScale.x/2,objectScale.y/2);
    
    //glm::vec2 distance = glm::abs(posToCheckTo) - (objectScale);
    //std::cout << distance.x << " " << distance.y << "\n";
    return glm::length(glm::max(distance,0.0f)) + glm::min(glm::max(distance.x, distance.y),0.0f);
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

bool CollisionTester::areEntitiesColliding(Entity* e1, Entity* e2)
{
    if(!simpleCheckForCollision(e1, e2))
        return false;
    
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
    
    return true;
}

bool CollisionTester::simpleCheckForCollision(Entity* e1, Entity* e2)
{
    float widthBoundingBoxE1, widthBoundingBoxE2;
    float gapXbetweeenE1E2, gapYbetweeenE1E2;
    
    //BoundingBox *1.2f because on unevenly Scales Boxes it sometimes is an Situation between perf/Precision
    if(e1->getScale().x >= e1->getScale().y)
        widthBoundingBoxE1 = e1->getScale().x*1.2f; // 1.2
    else 
        widthBoundingBoxE1 = e1->getScale().y*1.2f;
    
    if(e2->getScale().x >= e2->getScale().y)
        widthBoundingBoxE2 = e2->getScale().x*1.2f;
    else
        widthBoundingBoxE2 = e2->getScale().y*1.2f;

    //X-GAP based on Position
    if(e1->getPosition().x <= e2->getPosition().x)
        gapXbetweeenE1E2 = (e2->getPosition().x - widthBoundingBoxE2) - (e1->getPosition().x + widthBoundingBoxE1);
    else 
        gapXbetweeenE1E2 = (e1->getPosition().x - widthBoundingBoxE1) - (e2->getPosition().x + widthBoundingBoxE2);
    
    //Y-GAP based on Position
    if(e1->getPosition().y <= e2->getPosition().y)
        gapYbetweeenE1E2 = (e2->getPosition().y - widthBoundingBoxE2) - (e1->getPosition().y + widthBoundingBoxE1);
    else 
        gapYbetweeenE1E2 = (e1->getPosition().y - widthBoundingBoxE1) - (e2->getPosition().y + widthBoundingBoxE2);

    //std::cout << "gapX: " << gapXbetweeenE1E2 << "\n";
    //std::cout << "gapY: " << gapYbetweeenE1E2 << "\n";
    if((gapXbetweeenE1E2 <= 0) && (gapYbetweeenE1E2 <= 0))
        return true;
    return false;
}
