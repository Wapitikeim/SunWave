#include "MortonHashTableLogic.h"
#include "../components/PhysicsCollider.h"
#include <iostream>
#include <algorithm>

auto MortonHashTableLogic::interleaveBits(const uint16_t &intToInterleave)
{
    uint32_t x = intToInterleave;
    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;
    return x;
}

uint32_t MortonHashTableLogic::mortonEncode2D(const int &bucketX, const int &bucketY)
{
    // Shift negative coordinates to non-negative space
    uint16_t xBits = static_cast<uint16_t>(bucketX + OFFSET);
    uint16_t yBits = static_cast<uint16_t>(bucketY + OFFSET);

    // Interleave bits to create Morton code
    return interleaveBits(xBits) | (interleaveBits(yBits) << 1);
}

void MortonHashTableLogic::removeColliderFromHashTable(PhysicsCollider* ref,std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> &table)
{
    for(auto& mortonEntry : ref->getTableIndicies())
    {
        auto& refTable = table[mortonEntry];
        refTable.erase(std::remove(refTable.begin(), refTable.end(), ref), refTable.end());
    }
    ref->clearIndiciesForHashTableOfThisEntity();
}

void MortonHashTableLogic::addColliderIntoHashTable(PhysicsCollider *ref, std::unordered_map<uint32_t, std::vector<PhysicsCollider *>> &table)
{
    removeColliderFromHashTable(ref, table);

    addCornerIndiciesInMortonEncode(ref);

    // Handle X scale
    if(BUCKETSIZE < (ref->getBody().colliderScale.x * 2))
        addMortonCodesForXScale(ref->getCornerPos().leftBottom, ref->getCornerPos().rightBottom, ref);

    // Handle Y scale
    if(BUCKETSIZE < (ref->getBody().colliderScale.y * 2))
        addMortonCodesForYScale(ref->getCornerPos().leftBottom, ref->getCornerPos().leftTop, ref);

    for(auto& mortonEntry : ref->getTableIndicies())
        table[mortonEntry].push_back(ref);
}

void MortonHashTableLogic::addCornerIndiciesInMortonEncode(PhysicsCollider* ref)
{
    for(auto& corner : ref->getCornerPos().cornerVec)
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((corner.x/BUCKETSIZE), (corner.y/BUCKETSIZE)));
}

void MortonHashTableLogic::addMortonCodesForXScale(glm::vec3& leftBot, glm::vec3& rightBot, PhysicsCollider* ref)
{
    glm::vec2 directionBot = glm::normalize(rightBot - leftBot);
    glm::vec2 directionTop = glm::normalize(ref->getCornerPos().rightTop - ref->getCornerPos().leftTop);
    float distanceBot = glm::distance(leftBot, rightBot);

    glm::vec2 currentBot = glm::vec2(leftBot) + directionBot * BUCKETSIZE;
    glm::vec2 currentTop = glm::vec2(ref->getCornerPos().leftTop) + directionTop * BUCKETSIZE;
    
    float adjustedBucketSize = BUCKETSIZE/CORRECTION_DIVIDER;
    distanceBot -= adjustedBucketSize;

    while (distanceBot > BUCKETSIZE)
    {
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentBot.x/BUCKETSIZE), (currentBot.y/BUCKETSIZE)));
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentTop.x/BUCKETSIZE), (currentTop.y/BUCKETSIZE)));
        currentBot += directionBot * adjustedBucketSize;
        currentTop += directionTop * adjustedBucketSize;
        distanceBot -= adjustedBucketSize;
    }
}

void MortonHashTableLogic::addMortonCodesForYScale(glm::vec3& leftBot, glm::vec3& leftTop, PhysicsCollider* ref)
{
    glm::vec2 directionLeft = glm::normalize(leftTop - leftBot);
    glm::vec2 directionRight = glm::normalize(ref->getCornerPos().rightTop - ref->getCornerPos().rightBottom);
    float distanceLeft = glm::distance(leftBot, leftTop);

    glm::vec2 currentLeft = glm::vec2(leftBot) + directionLeft * BUCKETSIZE;
    glm::vec2 currentRight = glm::vec2(ref->getCornerPos().rightBottom) + directionRight * BUCKETSIZE;
    
    float adjustedBucketSize = BUCKETSIZE/CORRECTION_DIVIDER;
    distanceLeft -= adjustedBucketSize;

    while (distanceLeft > BUCKETSIZE)
    {
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentLeft.x/BUCKETSIZE), (currentLeft.y/BUCKETSIZE)));
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentRight.x/BUCKETSIZE), (currentRight.y/BUCKETSIZE)));
        currentLeft += directionLeft * adjustedBucketSize;
        currentRight += directionRight * adjustedBucketSize;
        distanceLeft -= adjustedBucketSize;
    }
}

int MortonHashTableLogic::getColliderOccurrences(PhysicsCollider* ref, const std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& mortonHashTable) const
{
    //Checks all entrys - kinda slow
    int totalOccurrences = 0;
    for (const auto& entry : mortonHashTable) 
        totalOccurrences += std::count(entry.second.begin(), entry.second.end(), ref);
    return totalOccurrences;
}

void MortonHashTableLogic::printColliderOccurrences(PhysicsCollider* ref, const std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& mortonHashTable) const
{
    int totalOccurrences = 0;
    for (const auto& entry : mortonHashTable) 
    {
        int count = std::count(entry.second.begin(), entry.second.end(), ref);
        if (count > 0) {
            std::cout << "Key: " << entry.first << " has " << count << " occurrences of ref." << std::endl;
            totalOccurrences += count;
        }
    }
    std::cout << "Total occurrences of ref in table: " << totalOccurrences << std::endl;
}
