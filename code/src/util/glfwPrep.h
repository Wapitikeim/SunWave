#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

class glfwPrep 
{
    private:
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

    public:
        static GLFWwindow* prepGLFWAndGladThenGiveBackWindow(unsigned int width, unsigned int height, const std::string &name);
};