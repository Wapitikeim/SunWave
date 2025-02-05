## Key Operations

### Window Creation
```cpp
// Create OpenGL 3.3 window with icon
GLFWwindow* window = glfwPrep::prepGLFWAndGladThenGiveBackWindow(
    1920,           // width
    1080,           // height
    "Window Title"  // window name
);
```

### Window Size Tracking
```cpp
// Get current window dimensions
int width = glfwPrep::getCurrentWindowWidth();
int height = glfwPrep::getCurrentWindowHeight();
```

## Key Features
1. OpenGL 3.3 core profile setup
2. GLAD initialization
3. Window icon loading
4. Viewport management
5. Framebuffer resize handling

Used by:
- [[GameEnvironment]]
- [[UiElement]]
- [[Camera]]