## Core Purpose
Static utility class for collision detection between 2D boxes using Separating Axis Theorem (SAT).

## Key Methods

### Primary Collision Tests
```cpp
// Basic collision check between two colliders
static bool arePhysicsCollidersColliding(PhysicsCollider* e1, PhysicsCollider* e2);

// Detailed collision info including contact normal and penetration
static bool arePhysicsCollidersCollidingWithDetails(
    PhysicsCollider* e1, 
    PhysicsCollider* e2, 
    glm::vec3& contactNormal, 
    float& penetrationDepth
);

// Test collision with a hypothetical collider "shell"
static bool isColliderCollidingWithShell(
    PhysicsCollider* e1, 
    glm::vec3& posE2, 
    glm::vec3& scaleE2, 
    float& rotzE2
);
```

### Distance Calculations
```cpp
// Signed distance between point and box
static float signedDistancePointAnd2DBox(
    glm::vec3 pointPos, 
    PhysicsCollider* colliderToCheck
);

// Signed distance between two boxes
static float signedDistance2DBoxAnd2DBox(
    PhysicsCollider* colliderOne, 
    PhysicsCollider* colliderTwo
);
```

## Implementation Details

### SAT Algorithm Steps
1. Calculate corner points for both colliders
2. Generate projection axes from edges
3. Project vertices onto each axis
4. Test for overlap on all axes
5. Calculate minimum penetration depth
6. Determine contact normal

## Key Features
1. Handles rotated boxes
2. Provides detailed collision information
3. 2D-focused implementation
4. Static utility design

## Related 
- [[MortonHashTableLogic]]