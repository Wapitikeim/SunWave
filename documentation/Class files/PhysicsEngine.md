## Core Overview

The PhysicsEngine manages physics simulation, collision detection, and resolution for physics colliders. It uses a morton encoded hash table for broad collision phase and SAT for narrow.

## Key Components

### Physics Object Management
```C++
// Register a collider with the physics engine
void registerPhysicsCollider(PhysicsCollider* colliderToRegister);

// Remove a collider from the physics engine
void unregisterCollider(PhysicsCollider* colliderToRemove);

// Clear all physics objects
void clearPhysicsObjects();
```
### Physics Simulation Control
```C++
// Control simulation speed and quality
float speedOfSimulation = 1;        // Default speed multiplier
float tickrateOfSimulation = 150;   // Physics updates per second

// Pause/resume physics simulation
void setIsHalting(const bool& newHalt);

// Get simulation step time
float getTimeStep() { return (1.0f / tickrateOfSimulation) * speedOfSimulation; }
```
### Collision Detection from outside the physicsengine
```C++
// Check if an object would collide at given position
bool checkIfShellWouldCollide(glm::vec3& pos, glm::vec3& scale, float& rotZ);

// Check collision between named entities
bool checkTriggerColliderCollision(const std::string& colliderEntityName, 

                                 const std::string& triggerColliderEntityName);

// Check collision with player
bool checkColliderPlayerCollision(const std::string& colliderEntityName);
```
## Constants & Parameters

### Physics Properties
```C++
float GRAVITY = -9.81f;           // Gravity force
float AIR_RESISTANCE = 0.995f;    // Air resistance multiplier
float RESTING_THRESHOLD = 0.15f;  // Minimum velocity for movement
float BOUNCE_MULTIPLIER = 2.25f;  // Collision bounce force multiplier
```
## Common Use Cases

### Setting Up a Physical Object
```C++
// Create and register a physical object
auto physicsCollider = std::make_unique<PhysicsCollider>(entity, boolIfStaticOrNot);
physicsCollider->setElasticity(0.5f);      // Set bounce factor
physicsCollider->setMass(mass);       // adjust the mass
physicsEngine->registerPhysicsCollider(physicsCollider.get());
```
### Collision Response Configuration
```C++
// Configure collision properties
physicsCollider->setIsTrigger(true);       // Trigger only, no physical response
physicsCollider->setUnaffectedByGravity(true);  // Floating object
physicsCollider->setLockX(true);           // Constrain X-axis movement
```
### Performance Monitoring
```C++
// Get physics engine statistics
int collisionsLastFrame = getTicksLastFrame();
int collisionsPerSecond = getTicksLastSecond();
int activeCollisions = getCurrentCollisions();
```
## Important Notes

1. Objects must be registered before physics simulation affects them
2. Trigger colliders don't cause physical responses but register as collisions
3. Static objects don't move but affect dynamic objects
4. Performance depends on number of active colliders
5. Higher tickrates increase accuracy but cost performance

## Best Practices

1. Use static colliders for immobile objects
2. Set appropriate mass values for realistic behavior
3. Use triggers for detection without physical response
4. Clear unused colliders to maintain performance
5. Balance tickrate with performance requirements

## Physics Steps

1. Broad phase (Morton hash table spatial partitioning)
2. Narrow phase (precise collision detection through Seperated Axis Theorem (SAT) )
3. Collision response (impulse-based resolution)
4. Position correction (prevent sinking)
5. Force integration (gravity, velocity updates)

## Related Components

- [[PhysicsCollider]]
- [[CollisionTester]]
- [[MortonHashTableLogic]]
- [[Entity]]