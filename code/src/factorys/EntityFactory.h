#pragma once
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "../entities/Entity.h"
#include "../entities/Shape.h"
#include "../entities/PlayerShape.h"
#include "../entities/UiElement.h"

std::unique_ptr<Entity> createEntity(const std::string& type, const std::string& name, const glm::vec3& position, const glm::vec3& scale, float rotation, const std::string& shaderName);
std::unique_ptr<Entity> createUiEntity(const std::string& type, const std::string& name, const glm::vec3& position, const glm::vec3& scale, float rotation, const std::string& textToBeRenderd, const std::string& fontName, unsigned int fontSize, glm::vec4& textColor);