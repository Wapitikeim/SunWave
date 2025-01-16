#include "EntityFactory.h"

std::unique_ptr<Entity> createEntity(const std::string& type, const std::string& name, const glm::vec3& position, const glm::vec3& scale, float rotation, const std::string& shaderName)
{
    if (type == "Shape")
    {
        return std::make_unique<Shape>(name, position, scale, rotation, true, shaderName);
    }
    else if (type == "PlayerShape")
    {
        return std::make_unique<PlayerShape>(name, position, scale, rotation, true, shaderName);
    }
    else if (type == "UiElement")
    {
        //UiElement(std::string elementName, glm::vec3 elementPosition, glm::vec3 elementScale, float elementRotation, std::string textToBeRenderd, std::string fontName, unsigned int fontSize);
        return std::make_unique<UiElement>(name, position, scale, rotation, "Factory needs to be adjusted", "Open_Sans\\OpenSans.ttf", 48);
    }
    // Add more entity types as needed
    else
    {
        throw std::runtime_error("Unknown entity type: " + type);
    }
}