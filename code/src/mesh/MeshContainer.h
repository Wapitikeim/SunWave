#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h>

#include <vector>
#include <memory>

#include "../util/fileReader.h"
#include "../ui/FontLoader.h"


class MeshContainer
{
    private:
        unsigned int VAO;
        unsigned int VBO;
        unsigned int EBO;
        unsigned int texture;

        unsigned int trianglesToDraw = 0;

        std::string textureName;

        void setupMesh();

        void setupVAO();
        void setupVBO();
        void setupVertexAttributeInterpretation();
        void setupEBO();
        void setupTexture();

        void setupForTextRendering();

        void generateBuffers();

    public:
        //GETTER/SETTER FEHLEN!
        
        std::shared_ptr<std::vector<float>> vertexAttributes;
        std::shared_ptr<std::vector<int>> indicesForEBO;
        
        MeshContainer(std::shared_ptr<std::vector<float>> vertexAttributesPointer, std::shared_ptr<std::vector<int>> indicesForEBOPointer, const std::string &textureNameRef)
        :vertexAttributes(vertexAttributesPointer), indicesForEBO(indicesForEBOPointer), textureName(textureNameRef)
        {
            //Default just position data for local Space
            //And Texture coord
            setupMesh();
        }
        MeshContainer(std::shared_ptr<std::vector<float>> vertexAttributesPointer, const unsigned int& trianglesToDrawRef, const std::string &textureNameRef)
        :vertexAttributes(vertexAttributesPointer), trianglesToDraw(trianglesToDrawRef), textureName(textureNameRef)
        {
            //Default just position data for local Space
            //And Texture coord
            setupMesh();
        }
        MeshContainer(bool textRendering)
        {
            if(textRendering)
                setupForTextRendering();
            else
                std::cout << "TextRendering was set to 0\n";
        }

        //Delete Copy Constructor
        MeshContainer(const MeshContainer &mesh) = delete;

        //Move Constructor
        MeshContainer(MeshContainer &&other) noexcept
        {
            std::swap(this->VAO, other.VAO);
            std::swap(this->VBO, other.VBO);
            std::swap(this->EBO, other.EBO);
            std::swap(this->texture, other.texture);
            std::swap(this->trianglesToDraw, other.trianglesToDraw);

            std::swap(this->textureName, other.textureName);
            std::swap(this->vertexAttributes, other.vertexAttributes);
        
            std::swap(this->indicesForEBO, other.indicesForEBO);
        }

        ~MeshContainer()
        {
            glDeleteVertexArrays(1,&VAO);
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteTextures(1, &texture);
        }
        
        void drawLine();
        void drawMesh();
        void drawText(std::string& textToBeRenderd, FontLoader& font,float x, float y, const float& scaleX, const float& scaleY);

        void reinitMesh();

        void setTexture(std::string newTextureName);

};
