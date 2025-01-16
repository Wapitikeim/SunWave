#include "UiElement.h"

UiElement::UiElement(std::string elementName, glm::vec3 elementPosition, glm::vec3 elementScale, float elementRotation, std::string textToBeRenderd, std::string fontName, unsigned int fontSize)
:Entity(std::move(elementName),ShaderContainer("textVertexShader.glsl", "textFragmentShader.glsl"),MeshContainer(1), elementPosition, elementScale, elementRotation),
textToBeRenderd(textToBeRenderd),font(FontLoader(fontName,fontSize)), currentFontName(fontName), currentFontSize(fontSize), textColor(glm::vec4(1))
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
