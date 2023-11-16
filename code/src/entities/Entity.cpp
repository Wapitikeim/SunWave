#include "Entity.h"

void Entity::draw()
{
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