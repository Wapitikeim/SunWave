#include "UiElement.h"
#include "../components/PhysicsCollider.h"

glm::vec2 UiElement::calculateTextSize()
{
    float width = 0.0f;
    float height = 0.0f;

    for (const char& c : textToBeRenderd)
    {
        Character ch = font.getCharacters().at(c);
        width += (ch.Advance >> 6); // Bitshift by 6 to get value in pixels (2^6 = 64)
        height = std::max(height, static_cast<float>(ch.Size.y));
    }
    width/=50;
    height/=50;
    return glm::vec2(width, height);
}

UiElement::UiElement(std::string elementName, glm::vec3 elementPosition, glm::vec3 elementScale, float elementRotation, std::string textToBeRenderd, std::string fontName, unsigned int fontSize)
    : Entity(std::move(elementName), ShaderContainer("textVertexShader.glsl", "textFragmentShader.glsl"), MeshContainer(1), elementPosition, elementScale, elementRotation),
      textToBeRenderd(textToBeRenderd), font(FontLoader(fontName, fontSize)), currentFontName(fontName), currentFontSize(fontSize), textColor(glm::vec4(1))
{
    type = "UiElement";
    shaderName = "textFragmentShader";
}

void UiElement::draw()
{
    if(dontDraw)
        return;
    entityShader.useShader();
    entityShader.setUniformVec4("colorChange", textColor);
    entityShader.setUniformMat4("textProjection", Camera::getCurrentCameraOrto());

    glm::vec4 clipSpacePos = Camera::getCurrentCameraProjection() * Camera::getCurrentCameraView() * glm::vec4(entityPosition, 1.0f);
    glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos) / clipSpacePos.w;

    glm::vec2 screenSpacePos;
    screenSpacePos.x = (ndcSpacePos.x + 1.0f) * 0.5f * 1920.0f;
    screenSpacePos.y = (ndcSpacePos.y + 1.0f) * 0.5f * 1080.0f;


    entityMesh.drawText(textToBeRenderd, font, screenSpacePos.x, screenSpacePos.y, entityScale.x*entityScale.y);
    
}

void UiElement::update(GLFWwindow *window, float deltaTime)
{
    
    for(auto &comp:this->componentsOfThisEntity)
    {
        comp->update();
        if (auto physicsCollider = dynamic_cast<PhysicsCollider*>(comp.get()))
        {
            glm::vec2 textScale = glm::vec2(calculateTextSize());
            physicsCollider->setScale(glm::vec3(textScale.x,textScale.y, physicsCollider->getColliderScale().z));
        }
        
    }
}
