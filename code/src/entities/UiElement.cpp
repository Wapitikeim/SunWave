#include "UiElement.h"
#include "../components/PhysicsCollider.h"

glm::vec2 UiElement::calculateTextSize()
{
    float width = 0.0f;
    float maxHeight = 0.0f;

    for (const char& c : textToBeRenderd)
    {
        Character ch;
        try
        {
            ch = font.getCharacters().at(c);
        }
        catch (const std::exception& e)
        {
            ch = font.getCharacters().at('?');
        }
        width += (ch.Advance >> 6) * entityScale.x; // Apply the x scale factor
        maxHeight = std::max(maxHeight, static_cast<float>(ch.Size.y) * entityScale.y); // Apply the y scale factor
    }

    return glm::vec2(width, maxHeight);
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

    // Construct the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, entityPosition);
    model = glm::rotate(model, glm::radians(entityRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, entityScale);

    // Pass the model matrix to the shader
    entityShader.setUniformMat4("model", model);

    glm::vec4 clipSpacePos = Camera::getCurrentCameraProjection() * Camera::getCurrentCameraView() * glm::vec4(entityPosition, 1.0f);
    glm::vec3 ndcSpacePos = glm::vec3(clipSpacePos) / clipSpacePos.w;

    glm::vec2 screenSpacePos;
    screenSpacePos.x = (ndcSpacePos.x + 1.0f) * 0.5f * 1920.0f;
    screenSpacePos.y = (ndcSpacePos.y + 1.0f) * 0.5f * 1080.0f;
    //XPos muss nach positiv korrigiert werden basierend auf TextLänge/2
    //YPos basierend auf Höhe/2
    glm::vec2 textSize = calculateTextSize();
    screenSpacePos.x -= textSize.x/2;
    screenSpacePos.y -= textSize.y/2;

    entityMesh.drawText(textToBeRenderd, font, screenSpacePos.x, screenSpacePos.y, entityScale.x);

}

