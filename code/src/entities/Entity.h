#pragma once

#include <string>
#include <glm/vec4.hpp>

#include "../shaders/ShaderContainer.h"
#include "../mesh/MeshContainer.h"
#include "../util/Camera.h"
#include "../components/Component.h"

//class GameEnviorment;
class Entity 
{
    private:
        //Unique Identifier
        std::string entityName;
        
    protected:
        std::string type = "Entity";
        std::string shaderName;
        std::vector<std::unique_ptr<Component>> componentsOfThisEntity;
        
        //World Data 
        glm::vec3 entityScale;
        float entityRotation;
        glm::vec3 entityPosition;
        
        //Components
        bool dontDraw = false;
        ShaderContainer entityShader;
        MeshContainer entityMesh;
        

    public:
        Entity(std::string Name, ShaderContainer shader, MeshContainer mesh, glm::vec3 position, glm::vec3 scale, float rotation )
        :entityName(std::move(Name)), entityShader(std::move(shader)), entityMesh(std::move(mesh)), entityPosition(position), entityScale(scale), entityRotation(rotation)
        {};
        
        virtual ~Entity() = default;

        virtual void update(GLFWwindow* window, float deltaTime);
        virtual void draw();

        //Getter
        [[nodiscard]] const glm::vec3 &getScale() const { return entityScale; }
        [[nodiscard]] float getRotation() const { return entityRotation; }
        [[nodiscard]] const glm::vec3 &getPosition() const {return entityPosition; }
        [[nodiscard]] const ShaderContainer &getShaderContainer() const {return entityShader;}
        [[nodiscard]] const MeshContainer &getMeshContainer() const {return entityMesh;};
        [[nodiscard]] const bool &getDontDraw() const {return dontDraw;}
        [[nodiscard]] const std::string &getEntityName() const {return entityName;};
        [[nodiscard]] const std::string &getEntityType() const {return type;};
        [[nodiscard]] const std::string &getShaderName() const {return shaderName;};
        
        //Setter
        void setScale(const glm::vec3 &scale) { Entity::entityScale = scale; }
        void setZRotation(float zRotation) { Entity::entityRotation = zRotation; }
        void setPosition(const glm::vec3 &position) { Entity::entityPosition = position; }
        void setDontDraw(const bool &newIsDisabeld){dontDraw = newIsDisabeld;};
        void switchTexture(std::string newTextureName);

        //Component Functions
        void addComponent(std::unique_ptr<Component> componentToAdd)
        {
            bool checkIfComponentExist = false;
            for(auto& componentName : componentsOfThisEntity)
                if(componentName->getName() == componentToAdd->getName())
                    checkIfComponentExist = true;
                
            if(!checkIfComponentExist)
                componentsOfThisEntity.push_back(std::move(componentToAdd));
            else
                std::cout << "The Component: " <<componentToAdd->getName() << " on " << this->getEntityName() << " cannot be added it already has one.\n";
            
        };
        Component* getComponent(std::string componentName);
        void removeComponent(std::string componentName);
        int getComponentArraySize(){return componentsOfThisEntity.size();};

};