#pragma once 
#include "Entity.h"
#include "../ui/FontLoader.h"

class UiElement : public Entity
{
    private:
        //MeshStuff
        std::shared_ptr<std::vector<float>> quadrat;
        
        std::shared_ptr<std::vector<int>> EBOIndices;

        std::string textToBeRenderd;
        FontLoader font;

    public:
        UiElement(std::string elementName, glm::vec3 elementPosition, glm::vec3 elementScale, float elementRotation, std::string textToBeRenderd, std::string fontName, unsigned int fontSize);
        void draw() override;
};
