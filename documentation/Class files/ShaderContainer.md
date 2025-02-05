## Core Purpose
OpenGL shader program management including compilation, linking, and uniform handling.

## Key Components

### Shader Setup
```cpp
// Create shader program with vertex and fragment shaders
auto shader = ShaderContainer(
    "vertex.glsl",     // Vertex shader path
    "fragment.glsl"    // Fragment shader path
);
```

### Uniform Management
```cpp
// Set common uniforms
shader.useShader();
shader.setUniformVec4("color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
shader.setUniformMat4("projection", projectionMatrix);
shader.setUniformFloat("time", glfwGetTime());
```

## Key Features
1. RAII shader management
2. Move semantics (no copying)
3. Automatic error checking
4. Uniform value setting

## Error Handling
```cpp
struct glStatusData {
    int success;
    const char* shaderName;
    char infoLog[GL_INFO_LOG_LENGTH];
};
```

Used by:
- [[Entity]]
- [[UiElement]]
- [[MeshContainer]]

Note: Requires valid OpenGL context and shader files in correct path