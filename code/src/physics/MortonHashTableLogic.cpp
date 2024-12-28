#include "MortonHashTableLogic.h"
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

void MortonHashTableLogic::prepareIndices(PhysicsCollider* ref)
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
    distanceBot -= BUCKETSIZE;

    while (distanceBot > BUCKETSIZE)
    {
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentBot.x/BUCKETSIZE), (currentBot.y/BUCKETSIZE)));
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentTop.x/BUCKETSIZE), (currentTop.y/BUCKETSIZE)));
        currentBot += directionBot * BUCKETSIZE;
        currentTop += directionTop * BUCKETSIZE;
        distanceBot -= BUCKETSIZE;
    }
}

void MortonHashTableLogic::addMortonCodesForYScale(glm::vec3& leftBot, glm::vec3& leftTop, PhysicsCollider* ref)
{
    glm::vec2 directionLeft = glm::normalize(leftTop - leftBot);
    glm::vec2 directionRight = glm::normalize(ref->getCornerPos().rightTop - ref->getCornerPos().rightBottom);
    float distanceLeft = glm::distance(leftBot, leftTop);

    glm::vec2 currentLeft = glm::vec2(leftBot) + directionLeft * BUCKETSIZE;
    glm::vec2 currentRight = glm::vec2(ref->getCornerPos().rightBottom) + directionRight * BUCKETSIZE;
    distanceLeft -= BUCKETSIZE;

    while (distanceLeft > BUCKETSIZE)
    {
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentLeft.x/BUCKETSIZE), (currentLeft.y/BUCKETSIZE)));
        ref->addOneIndexIntoIndiciesForHashTable(mortonEncode2D((currentRight.x/BUCKETSIZE), (currentRight.y/BUCKETSIZE)));
        currentLeft += directionLeft * BUCKETSIZE;
        currentRight += directionRight * BUCKETSIZE;
        distanceLeft -= BUCKETSIZE;
    }
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
