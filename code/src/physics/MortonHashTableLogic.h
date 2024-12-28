#pragma once 
#include "../components/PhysicsCollider.h"
#include <glm/glm.hpp>
#include <map>
#include <unordered_map>
#include <algorithm>


class MortonHashTableLogic 
{
    private:
        auto interleaveBits(const uint16_t& intToInterleave);
        uint32_t mortonEncode2D(const int& bucketX, const int& bucketY);
        

    public:
        MortonHashTableLogic() = default;
        ~MortonHashTableLogic() = default;
        const int32_t OFFSET = 0;
        const float BUCKETSIZE = 2.f; //2.f Bucketsize = 1.f Scale

        void removeColliderFromHashTable(PhysicsCollider* ref, std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> &table);
        void prepareIndices(PhysicsCollider* ref);
        void addMortonCodesForXScale(glm::vec3 &leftBot, glm::vec3 &rightBot, PhysicsCollider* ref);
        void addMortonCodesForYScale(glm::vec3 &leftBot, glm::vec3 &leftTop, PhysicsCollider* ref);
};