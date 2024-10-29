#include "MeshContainer.h"


void MeshContainer::setupMesh()
{
    generateBuffers();
    if(vertexAttributes)
    {
        setupVAO();
        setupVBO();
        setupVertexAttributeInterpretation();
        if(indicesForEBO)
            setupEBO();
        setupTexture();
    }
}

void MeshContainer::setupVAO()
{
    //VAO
    glBindVertexArray(VAO);
}
void MeshContainer::setupVBO()
{
    //VBO Vertex Buffer Object to send to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexAttributes->capacity()*4, &vertexAttributes->operator[](0), GL_STATIC_DRAW);
}
void MeshContainer::setupVertexAttributeInterpretation()
{
    //Vertex Attribute Interpretation
    //Pos
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 5*sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    //TexCord
    glVertexAttribPointer(1,2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1);
   
   
    

    /* //Color
    glVertexAttribPointer(1,3,GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3* sizeof(float)));
    glEnableVertexAttribArray(1); */
    
}
void MeshContainer::setupEBO()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesForEBO->capacity()*4, &indicesForEBO->operator[](0), GL_STATIC_DRAW);
}
void MeshContainer::setupTexture()
{
    //Texture
    glBindTexture(GL_TEXTURE_2D, texture);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels = 0;


    //UGLY
    if(textureName.find(".png") != std::string::npos)
        stbi_set_flip_vertically_on_load(1);
    
    unsigned char *data = stbi_load(fileReader::getPathToFileInFolder(textureName, "assets").string().c_str(), &width, &height, &nrChannels, 0);
    if(data)
    {
        //If .jpg no alpha
        if(textureName.find(".jpg") != std::string::npos)
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {    
        std::cout << "Failed stb Loader something is wrong";
    }
    stbi_image_free(data);
}

void MeshContainer::generateBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);
    glGenTextures(1, &texture);
}

void MeshContainer::drawLine()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_LINES, indicesForEBO->capacity(), GL_UNSIGNED_INT, 0);
}
void MeshContainer::drawMesh()
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    if(indicesForEBO)
        glDrawElements(GL_TRIANGLES, indicesForEBO->capacity(), GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, trianglesToDraw);
    
}

void MeshContainer::reinitMesh()
{   
    setupVAO();
    setupVBO();
    setupVertexAttributeInterpretation();
    if(indicesForEBO)
        setupEBO();
    setupTexture();
}

void MeshContainer::setTexture(std::string newTextureName)
{
    textureName = newTextureName;
    reinitMesh();
}
