#pragma once

#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/vec4.hpp>
#include "../util/fileReader.h"

class ShaderContainer 
{
    private:
        
        unsigned int shaderID;
        unsigned int fragmentShaderID;
        unsigned int vertexShaderID;
        std::string fragmentShader;
        std::string vertexShader;

        void setupAndCompileShaders();
        void linkProgrammAndAttachShaders();
        void cleanUpShaders();
        void checkForShaderCompileError(unsigned int shaderToCheckFor, const char* shaderSource);

    public:
        ShaderContainer(std::string vertexShaderName, std::string fragmentShaderName)
        :vertexShader(std::move(vertexShaderName)), fragmentShader(std::move(fragmentShaderName))
        {
            setupAndCompileShaders();
            linkProgrammAndAttachShaders();
            cleanUpShaders();
        }

        virtual ~ShaderContainer()
        {
            cleanUpShaders();
            glDeleteProgram(shaderID);
        };
        //Delete Copy Constructor
        ShaderContainer(const ShaderContainer &shader) = delete;

        //Move Constructor
        ShaderContainer(ShaderContainer &&other) noexcept
        {
            std::swap(this->shaderID, other.shaderID);
            std::swap(this->fragmentShaderID, other.fragmentShaderID);
            std::swap(this->vertexShaderID, other.vertexShaderID);
            std::swap(this->fragmentShader, other.fragmentShader);
            std::swap(this->vertexShader, other.vertexShader);
        }

        unsigned int& getShaderID(){return fragmentShaderID;};
        void useShader();

        void setUniformVec4(const std::string &uniformName, const glm::vec4 &valuesToSet) const;

        void setUniformMat4(const std::string &uniformName, const glm::mat4 &valuesToSet) const;

        void setUniformFloat(const std::string &uniformName, const float &valueToSet) const;

};
