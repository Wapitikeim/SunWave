#pragma once
#include <memory>
#include <string>
#include <glm/vec3.hpp>
#include "../Entities/Entity.h"
#include "../Entities/Shape.h"
#include "../Entities/PlayerShape.h"

std::unique_ptr<Entity> createEntity(const std::string& type, const std::string& name, const glm::vec3& position, const glm::vec3& scale, float rotation, const std::string& shaderName);