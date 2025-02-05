## Core Purpose
Base class for all game objects that provides:
1. Transform functionality (position, rotation, scale)
2. Component system
3. Rendering capabilities
4. Unique identification (currently the name!)

## Class Structure

### Core Properties
```cpp
private:
    std::string entityName;       // Unique identifier
protected:
    std::string type;            // Entity type identifier
    glm::vec3 entityPosition;    // World position
    glm::vec3 entityScale;       // Object scale
    float entityRotation;        // Z-axis rotation
    ShaderContainer entityShader;// Rendering shader
    MeshContainer entityMesh;    // Visual mesh
```

### Component System
```cpp
// Add component
entity->addComponent(std::make_unique<CustomComponent>());

// Get component
auto* component = entity->getComponent("ComponentName");

// Remove component
entity->removeComponent("ComponentName");
```

### Transform Operations
```cpp
// Position
entity->setPosition(glm::vec3(x, y, z));
glm::vec3 pos = entity->getPosition();

// Rotation
entity->setZRotation(45.0f);
float rot = entity->getRotation();

// Scale
entity->setScale(glm::vec3(2.0f, 2.0f, 1.0f));
glm::vec3 scale = entity->getScale();
```

## Creating Custom Entities

### Basic Entity
```cpp
class MyEntity : public Entity {
public:
    MyEntity(const std::string& name, glm::vec3 pos) 
        : Entity(name, 
                ShaderContainer("basicShader"), 
                MeshContainer("basicMesh"), 
                pos,
                glm::vec3(1.0f),
                0.0f) {
        type = "MyEntity";
    }
    
    void update(GLFWwindow* window, float deltaTime) override {
        Entity::update(window, deltaTime);
        // Custom update logic
    }
};
```

### Component-Based Entity
```cpp
// Create entity with components
auto entity = std::make_unique<Entity>("Player", shader, mesh, position, scale, rotation);
entity->addComponent(std::make_unique<PhysicsComponent>());
entity->addComponent(std::make_unique<InputComponent>());
```

## Best Practices
1. Keep in mind the name is the UUID
2. Check component existence before access
3. Override update() for custom behavior

## Common Use Cases

### Dynamic Object
```cpp
auto dynamicObject = std::make_unique<Entity>("Player",
    ShaderContainer("characterShader"),
    MeshContainer("playerMesh"),
    glm::vec3(0.0f),
    glm::vec3(1.0f),
    0.0f
);
dynamicObject->addComponent(std::make_unique<PhysicsCollider>(dynamicObject.get(), false));
```

### Related Code
- [[PlayerShape]]
- [[UiElement]]