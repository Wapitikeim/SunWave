## Overview
Spatial partitioning system using Morton encoding (Z-order curve) for efficient broad-phase collision detection.

## Core Purpose
1. Converts 2D space into grid cells
2. Maps objects to grid using Morton codes
3. Enables quick spatial queries
4. Reduces collision check pairs

## Technical Implementation
```cpp
class MortonHashTableLogic {
private:
    const int32_t OFFSET = 50;        // Shift for negative coordinates
    const float BUCKETSIZE = 2.0f;    // Grid cell size
    const float CORRECTION_DIVIDER = 2.0f;  // Subdivision factor
};
```

## Key Operations

### Morton Encoding
```cpp
// Convert 2D coordinates to Morton code
uint32_t mortonCode = mortonEncode2D(x, y);

// Add collider to spatial hash
void addColliderIntoHashTable(PhysicsCollider* collider, 
                            std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& table);

// Remove collider from spatial hash
void removeColliderFromHashTable(PhysicsCollider* collider,
                               std::unordered_map<uint32_t, std::vector<PhysicsCollider*>>& table);
```

## Integration with Physics Engine
```cpp
// In PhysicsEngine class
private:
    std::unordered_map<uint32_t, std::vector<PhysicsCollider*>> mortonHashTable;
    MortonHashTableLogic tableLogic;

// Usage in broad phase
void broadCollisionGathering() {
    // Objects in same grid cell are potential collision pairs
    for (auto& [key, colliders] : mortonHashTable) {
        if (colliders.size() > 1) {
            // Process potential collisions
        }
    }
}
```

## Performance Notes
1. Grid size (BUCKETSIZE) affects collision detection precision
2. OFFSET handles negative coordinate space
3. CORRECTION_DIVIDER prevents edge case misses
4. Efficient for uniformly distributed objects
5. O(n) space complexity for n objects

## Related 
- [[CollisionTester]]