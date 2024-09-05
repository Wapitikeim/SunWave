#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb_image.h> // For Icon
#include "fileReader.h"
#include <iostream>



class glfwPrep 
{
    private:
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static int currentWindowHeight;
        static int currentWindowWidth;

    public:
        static GLFWwindow* prepGLFWAndGladThenGiveBackWindow(unsigned int width, unsigned int height, const std::string &name);
        static int getCurrentWindowHeight(){return currentWindowHeight;};
        static int getCurrentWindowWidth(){return currentWindowWidth;};
};