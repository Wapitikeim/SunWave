#pragma once 
#include "Entity.h"
#include "../ui/FontLoader.h"
#include <functional>

class UiElement : public Entity
{
    private:
        glm::vec2 calculateTextSizeInPixels();
        glm::vec2 calculateTextSizeInWorldCoord();
        glm::vec4 textColor;
        std::string textToBeRenderd;
        std::string currentFontName;
        unsigned int currentFontSize;
        FontLoader font;

        //Button
        std::function<void()> onClickFunction;

    public:
        UiElement(std::string elementName, glm::vec3 elementPosition, glm::vec3 elementScale, float elementRotation, std::string textToBeRenderd, std::string fontName, unsigned int fontSize);
        void draw() override;
        void update(GLFWwindow* window, float deltaTime) override;
        FontLoader& getCurrentFont(){return font;};
        std::string& getCurrentFontName(){return currentFontName;};
        unsigned int& getCurrentFontSize(){return currentFontSize;};
        std::string& getText(){return textToBeRenderd;};
        glm::vec4& getTextColor(){return textColor;};
        void const setTextColor(glm::vec4& newColor){ textColor = newColor;};
        void const setTextToBeRenderd(std::string& newTextToBeRenderd){textToBeRenderd = newTextToBeRenderd;};
        void const setNewFont(std::string& newFontName, unsigned int newFontSize){font = FontLoader(newFontName, newFontSize);};

        void setOnClick(std::function<void()> callback) { onClickFunction = callback; }
        void click() { if (onClickFunction) onClickFunction(); }

};
