#pragma once

#include <string>
#include "../shaders/ShaderContainer.h"
#include "../mesh/MeshContainer.h"
#include "../util/Camera.h"

#include <glm/vec4.hpp>

class Entity 
{
private:
    //Misc for now
    std::string entityName;
    
    //Components
    ShaderContainer entityShader;
    
    

protected:
    //World Data 
    glm::vec3 entityScale;
    float entityRotation;
    glm::vec3 entityPosition;

    MeshContainer entityMesh;

public:
    Entity(std::string Name, ShaderContainer shader, MeshContainer mesh, glm::vec3 position, glm::vec3 scale, float rotation )
    :entityName(std::move(Name)), entityShader(std::move(shader)), entityMesh(std::move(mesh)), entityPosition(position), entityScale(scale), entityRotation(rotation)
    {};


    /* Entity(std::string Name, ShaderContainer shader, MeshContainer mesh, glm::vec3 position)
    :entityName(std::move(Name)), entityShader(std::move(shader)), entityMesh(std::move(mesh)), entityPosition(position)
    {
        entityScale = glm::vec3(1.0f, 1.0f, 1.0f);
        entityRotation = 0.0f;
    }; */

    //Delete Copy Constructor
    //Entity(const Entity &entity) = delete;
    
    //Move Constructor
    /* Entity(Entity &&other) noexcept
    {
        std::swap(this->entityName, other.entityName);

        std::swap(this->entityShader, other.entityShader);
        std::swap(this->entityMesh, other.entityMesh); 

        std::swap(this->entityScale, other.entityScale);
        std::swap(this->entityRotation, other.entityRotation);
        std::swap(this->entityPosition, other.entityPosition);
    } */
 

    virtual ~Entity() = default;

    virtual void update(GLFWwindow *window, float deltaTime) {};
    virtual void draw();

    //Getter/Setter
    [[nodiscard]] const glm::vec3 &getScale() const { return entityScale; }
    [[nodiscard]] float getRotation() const { return entityRotation; }
    [[nodiscard]] const glm::vec3 &getPosition() const {return entityPosition; }
    
    void setScale(const glm::vec3 &scale) { Entity::entityScale = scale; }
    void setZRotation(float zRotation) { Entity::entityRotation = zRotation; }
    void setPosition(const glm::vec3 &position) { Entity::entityPosition = position; }
};