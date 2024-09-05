#include "glfwPrep.h"

int glfwPrep::currentWindowHeight = 0;
int glfwPrep::currentWindowWidth = 0;

void glfwPrep::framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    glfwGetWindowSize(window, &currentWindowWidth, &currentWindowHeight);
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

    //Icon Set // Could divide in 16x16 and 32x32 
    GLFWimage images[1]; 
    images[0].pixels = stbi_load(fileReader::getPathToFileInFolder("ico.png", "assets").string().c_str(), &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images); 
    stbi_image_free(images[0].pixels);

    //GladInit
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    //OpenGLRendering Field (0,0) -> bottom left corner -> (800,600)-> top right
    glViewport(0, 0, width, height);
    //"z buffer" for Faces to get discarded if overlap
    glEnable(GL_DEPTH_TEST);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwGetWindowSize(window, &currentWindowWidth, &currentWindowHeight);

    return window;
}


