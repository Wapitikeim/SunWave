## Core Purpose
Base class for the entity component system. Used for extending entity functionality through composition.

## Example Usage
```cpp
// Custom component
class PhysicsComponent : public Component {
    public:
        PhysicsComponent() { componentName = "Physics"; }
        void update() override { /* Physics logic */ }
};

// Add to entity
entity->addComponent(std::make_unique<PhysicsComponent>());

// Get component
auto* physics = entity->getComponent("Physics");
```

## Key Features
1. Virtual interface for components
2. Name-based component identification 
3. Used by Entity system
4. Update mechanism for game loop integration

Used by:
- [[PhysicsCollider]]
- [[Entity]]