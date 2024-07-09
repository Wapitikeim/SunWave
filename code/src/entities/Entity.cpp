#include "Entity.h"

void Entity::draw()
{
    if(dontDraw)
        return;
    entityShader.useShader();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, entityPosition);
    model = glm::rotate(model, glm::radians(entityRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, entityScale);
    
    entityShader.setUniformMat4("model", model);
    entityShader.setUniformMat4("view", Camera::getCurrentCameraView());
    entityShader.setUniformMat4("projection", Camera::getCurrentCameraProjection());

    entityMesh.drawMesh();
}

void Entity::switchTexture(std::string newTextureName)
{
    entityMesh.setTexture(newTextureName);
}

Component* Entity::getComponent(std::string componentName)
{
    for(auto &component:componentsOfThisEntity)
    {
        if(component->getName()==componentName)
            return component.get();
    }
    return nullptr;  
}
void Entity::removeComponent(std::string componentName) 
{
    int componenttoDelete = -1;
    for(int i = 0; i < componentsOfThisEntity.size(); i++)
    {
        if(componentsOfThisEntity[i]->getName()==componentName)
            componenttoDelete = i;
    }
    if(componenttoDelete != -1)
        componentsOfThisEntity.erase(componentsOfThisEntity.begin() + componenttoDelete);

};
