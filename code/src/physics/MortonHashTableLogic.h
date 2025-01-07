#pragma once 
#include <glm/glm.hpp>
#include <unordered_map>

class PhysicsCollider;

class MortonHashTableLogic 
{
    private:
        auto interleaveBits(const uint16_t& intToInterleave);
        uint32_t mortonEncode2D(const int& bucketX, const int& bucketY);
        

    public:
        MortonHashTableLogic() = default;
        ~MortonHashTableLogic() = default;
        const int32_t OFFSET = 50;
        const float CORRECTION_DIVIDER = 2.0f; //Makes sure that for edge cases all relevant buckets are hit
        const float BUCKETSIZE = 2.0f; //2.f Bucketsize = 1.f Scale

        void removeColliderFromHashTable(PhysicsCollider* ref, std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> &table);
        void addColliderIntoHashTable(PhysicsCollider* ref, std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> &table);
        void addCornerIndiciesInMortonEncode(PhysicsCollider* ref);
        void addMortonCodesForXScale(glm::vec3 &leftBot, glm::vec3 &rightBot, PhysicsCollider* ref);
        void addMortonCodesForYScale(glm::vec3 &leftBot, glm::vec3 &leftTop, PhysicsCollider* ref);
        int getColliderOccurrences(PhysicsCollider* ref, const std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& mortonHashTable) const;
        void printColliderOccurrences(PhysicsCollider* ref, const std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& mortonHashTable) const;
};