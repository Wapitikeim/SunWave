#include "Camera.h"

//Init for static Member Var
glm::mat4 Camera::view = glm::mat4(1.0f);
glm::mat4 Camera::projection = glm::mat4(1.0f);
glm::mat4 Camera::orto = glm::ortho(0.0f, 1920.f, 0.0f, 1080.f);


glm::mat4 Camera::getCurrentCameraView()
{
    return Camera::view;
}

glm::mat4 Camera::getCurrentCameraProjection()
{
    return Camera::projection;
}

glm::mat4 Camera::getCurrentCameraOrto()
{
    return orto;
}

void Camera::setCurrentCameraView(const glm::mat4 &viewToSet)
{
    Camera::view = viewToSet;
}

void Camera::setCurrentCameraProjection(const glm::mat4 &projectionToSet)
{
    Camera::projection = projectionToSet;
}

void Camera::setCurrentCameraOrto(const glm::mat4 &ortoToSet)
{
    Camera::orto = ortoToSet;
}
