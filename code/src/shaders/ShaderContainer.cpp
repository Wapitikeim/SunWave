#include "ShaderContainer.h"

struct glStatusData 
{
    int success;
    const char *shaderName;
    char infoLog[GL_INFO_LOG_LENGTH];
};


void ShaderContainer::setupAndCompileShaders()
{
    //VertexShader
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    std::string& vertexShaderSource = fileReader::readShader(vertexShader);
    const char* vertexShaderSourceChar = vertexShaderSource.c_str();
    glShaderSource(vertexShaderID, 1, &vertexShaderSourceChar, NULL);
    glCompileShader(vertexShaderID);
    checkForShaderCompileError(vertexShaderID, vertexShaderSourceChar);


    //Fragment Shader
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    std::string& fragmentShaderSource = fileReader::readShader(fragmentShader);
    const char* fragmentShaderSourceChar = fragmentShaderSource.c_str();
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSourceChar, NULL);
    glCompileShader(fragmentShaderID);
    checkForShaderCompileError(fragmentShaderID, fragmentShaderSourceChar);

}

void ShaderContainer::linkProgrammAndAttachShaders()
{
    //Shader Programm (Who uses all Shaders)
    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShaderID);
    glAttachShader(shaderID, fragmentShaderID);
    
    glLinkProgram(shaderID);

    glDetachShader(shaderID, vertexShaderID);
    glDetachShader(shaderID, fragmentShaderID);
}

void ShaderContainer::cleanUpShaders()
{
    glDeleteShader(fragmentShaderID);
    glDeleteShader(vertexShaderID);
}

void ShaderContainer::checkForShaderCompileError(unsigned int shaderToCheckFor, const char *shaderSource)
{
    glStatusData compilationStatus{};
    compilationStatus.shaderName = shaderSource;
    
    glGetShaderiv(shaderToCheckFor, GL_COMPILE_STATUS, &compilationStatus.success);
    if(compilationStatus.success == GL_FALSE) 
    {
        glGetShaderInfoLog(shaderToCheckFor, GL_INFO_LOG_LENGTH, nullptr, compilationStatus.infoLog);
        throw std::runtime_error("ERROR: " + std::string(compilationStatus.shaderName) + " shader compilation failed.\n" +
            std::string(compilationStatus.infoLog));
    }
}

void ShaderContainer::useShader()
{
    glUseProgram(shaderID);
}



void ShaderContainer::setUniformVec4(const std::string &uniformName, const glm::vec4 &valuesToSet) const
{
    //Sets value in order x y z w
    glUniform4f(glGetUniformLocation(shaderID, uniformName.c_str()), valuesToSet.x, valuesToSet.y, valuesToSet.z, valuesToSet.w);
}

void ShaderContainer::setUniformMat4(const std::string &uniformName, const glm::mat4 &valuesToSet) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderID, uniformName.c_str()), 1, GL_FALSE, glm::value_ptr(valuesToSet));
}

void ShaderContainer::setUniformFloat(const std::string &uniformName, const float &valueToSet) const
{
    glUniform1f(glGetUniformLocation(shaderID, uniformName.c_str()),valueToSet);
}
