#pragma once

#include <vector>
#include <string>

#include "../entities/Entity.h"

#include "glm/glm.hpp"
class fieldOfShapes
{
    private:
    std::vector<std::unique_ptr<Entity>> currentShapeField;

    std::vector<std::string> shapeNames;
    glm::vec2 originToDrawFrom;
    glm::vec2 fieldDimensions;

    public:
    fieldOfShapes(std::vector<std::string> shapeNamesToUse, glm::vec2 origin, glm::vec2 dimensions)
    :shapeNames(std::move(shapeNamesToUse)), originToDrawFrom(std::move(origin)), fieldDimensions(std::move(dimensions))
    {
        generateField();
    };

    void generateField();
    void generateField(std::vector<Entity> shapesToKeep);

    void deleteCurrentField();

    void drawField();

    glm::vec2 getPosOfSolutionShape();
    std::unique_ptr<Entity> getSolutionShape();

};
