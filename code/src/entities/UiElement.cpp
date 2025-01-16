#include "UiElement.h"

UiElement::UiElement(std::string elementName, glm::vec3 elementPosition, glm::vec3 elementScale, float elementRotation, std::string textToBeRenderd, std::string fontName, unsigned int fontSize)
:Entity(std::move(elementName),ShaderContainer("textVertexShader.glsl", "textFragmentShader.glsl"),MeshContainer(1), elementPosition, elementScale, elementRotation),
textToBeRenderd(textToBeRenderd),font(FontLoader(fontName,fontSize))
{
    type = "UiElement";
    shaderName = "textFragmentShader";
}

void UiElement::draw()
{
    if(dontDraw)
        return;
    entityShader.useShader();
    entityShader.setUniformVec4("textColor", glm::vec4(1.f));
    entityShader.setUniformMat4("textProjection", Camera::getCurrentCameraOrto());

    entityMesh.drawText(textToBeRenderd, font, entityPosition.x, entityPosition.y, entityScale.x*entityScale.y);
    
}
