## Core Overview
PhysicsCollider is a component that adds physical properties and collision detection capabilities to entities.

## Key Structures

### PhysicsBody
```cpp
struct PhysicsBody {
    glm::vec3 colliderPosition;    // World position
    glm::vec3 colliderScale;       // Size/scale
    float colliderZRotation;       // Rotation around Z axis
    glm::vec3 colliderVelocity;    // Current velocity
    glm::vec3 colliderAcceleration;// Current acceleration
    glm::vec3 accumulatedForce;    // Sum of forces
    float mass;                    // Object mass
};
```

### CornerPositions
```cpp
struct CornerPositions {
    glm::vec3 leftBottom;
    glm::vec3 leftTop;
    glm::vec3 rightTop;
    glm::vec3 rightBottom;
    std::vector<glm::vec3> cornerVec;
};
```

## Usage

### Creating a PhysicsCollider
```cpp
// Create dynamic physics collider
auto entity = std::make_unique<Entity>("DynamicObject");
auto collider = std::make_unique<PhysicsCollider>(entity.get(), false);

// Create static physics collider
auto staticEntity = std::make_unique<Entity>("StaticObject");
auto staticCollider = std::make_unique<PhysicsCollider>(staticEntity.get(), true);
```

### Physics Properties
```cpp
// Configure physics behavior
collider->setMass(1.0f);
collider->setElascity(0.5f);           // Bounce factor (0-1)
collider->setUnaffectedByGravity(true);
collider->setIsTrigger(true);          // Collision detection only
```

### Force Application
```cpp
// Apply forces
collider->applyForce(glm::vec3(0.0f, 10.0f, 0.0f));  // Apply upward force
collider->clearForces();                              // Reset forces
```

### Movement Constraints
```cpp
collider->setLockX(true);   // Lock X-axis movement
collider->setLockY(true);   // Lock Y-axis movement
```

### Collision State
```cpp
bool isGrounded = collider->getIsGrounded();
bool isInContact = collider->getIsInContact();
bool isResting = collider->getIsResting();
```

## Properties

### Physical Properties
- Mass (default: 1.0)
- Elasticity (default: 0.2)
- Static/Dynamic state
- Trigger mode
- Grounded state
- Contact state
- Resting state

### Transform Data
- Position (World space)
- Scale (Size)
- Rotation (Z-axis only)
- Corner positions (For collision)

## Important Methods
```cpp
void update();                    // Update physics state
void updateCornerPositions();     // Recalculate collision bounds
void applyForce(glm::vec3);      // Add force to object
void clearForces();              // Reset accumulated forces
```

## Best Practices
1. Set appropriate mass for realistic physics
2. Use triggers for detection without physics
3. Mark static objects appropriately
4. Update transforms through physics engine
5. Clear forces when resetting object state

## Common Configurations

### Platform
```cpp
auto platform = std::make_unique<PhysicsCollider>(entity, true);
platform->setElascity(0.5f);
```

### Dynamic Object
```cpp
auto dynamic = std::make_unique<PhysicsCollider>(entity, false);
dynamic->setMass(1.0f);
dynamic->setElascity(0.2f);
```

### Trigger Zone
```cpp
auto trigger = std::make_unique<PhysicsCollider>(entity, true);
trigger->setIsTrigger(true);
trigger->setUnaffectedByGravity(true);
```