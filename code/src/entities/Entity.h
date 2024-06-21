#pragma once

#include <string>
#include "../shaders/ShaderContainer.h"
#include "../mesh/MeshContainer.h"
#include "../util/Camera.h"
#include "../components/Component.h"

#include <glm/vec4.hpp>


//class GameEnviorment;
class Entity 
{
    private:
        //Misc for now
        std::string entityName;
        
        //Components
        ShaderContainer entityShader;
        
    protected:
        std::vector<std::unique_ptr<Component>> componentsOfThisEntity;
        //World Data 
        glm::vec3 entityScale;
        float entityRotation;
        glm::vec3 entityPosition;

        MeshContainer entityMesh;

    public:
        Entity(std::string Name, ShaderContainer shader, MeshContainer mesh, glm::vec3 position, glm::vec3 scale, float rotation )
        :entityName(std::move(Name)), entityShader(std::move(shader)), entityMesh(std::move(mesh)), entityPosition(position), entityScale(scale), entityRotation(rotation)
        {};

        virtual ~Entity() = default;

        virtual void update(GLFWwindow* window, float deltaTime){};
        virtual void draw();

        //Getter/Setter
        [[nodiscard]] const glm::vec3 &getScale() const { return entityScale; }
        [[nodiscard]] float getRotation() const { return entityRotation; }
        [[nodiscard]] const glm::vec3 &getPosition() const {return entityPosition; }
        
        void setScale(const glm::vec3 &scale) { Entity::entityScale = scale; }
        void setZRotation(float zRotation) { Entity::entityRotation = zRotation; }
        void setPosition(const glm::vec3 &position) { Entity::entityPosition = position; }

        void switchTexture(std::string newTextureName);

        void addComponent(std::unique_ptr<Component> componentToAdd){componentsOfThisEntity.push_back(std::move(componentToAdd));};
        Component* getComponent(std::string componentName);
        void removeComponent(std::string componentName);

};