#include "glfwPrep.h"

void glfwPrep::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

GLFWwindow *glfwPrep::prepGLFWAndGladThenGiveBackWindow(unsigned int width, unsigned int height, const std::string &name)
{
    //GLFWInit | Prep
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    
    //Window Creation
    GLFWwindow* window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    //GladInit
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    //OpenGLRendering Field (0,0) -> bottom left corner -> (800,600)-> top right
    glViewport(0, 0, width, height);
    //"z buffer" for Faces to get discarded if overlap
    glEnable(GL_DEPTH_TEST);

    //Call Resize if Window Changes
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    return window;
}