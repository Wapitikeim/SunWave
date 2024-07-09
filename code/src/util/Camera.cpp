#include "Camera.h"

//Init for static Member Var
glm::mat4 Camera::view = glm::mat4(1.0f);
glm::mat4 Camera::projection = glm::mat4(1.0f);


glm::mat4 Camera::getCurrentCameraView()
{
    return Camera::view;
}

glm::mat4 Camera::getCurrentCameraProjection()
{
    return Camera::projection;
}

void Camera::setCurrentCameraView(const glm::mat4 &viewToSet)
{
    Camera::view = viewToSet;
}

void Camera::setCurrentCameraProjection(const glm::mat4 &projectionToSet)
{
    Camera::projection = projectionToSet;
}

