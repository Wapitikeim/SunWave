## Core Purpose
Handles OpenGL mesh data, buffers, and texture management for both 2D shapes and text rendering.

## Key Components

### Buffer Management
```cpp
private:
    unsigned int VAO;    // Vertex Array Object
    unsigned int VBO;    // Vertex Buffer Object
    unsigned int EBO;    // Element Buffer Object
    unsigned int texture;// Texture handle
```

### Creating Meshes
```cpp
// Standard mesh with texture
auto mesh = MeshContainer(
    vertexData,          // Vertex positions & UVs
    indices,             // Triangle indices
    "texture.png"        // Texture path
);

// Text rendering mesh
auto textMesh = MeshContainer(true);  // Special setup for text
```

### Drawing Operations
```cpp
// Regular mesh drawing
mesh.drawMesh();        // Textured triangles
mesh.drawLine();        // Line rendering

// Text rendering
mesh.drawText(
    "Hello World",      // Text to render
    fontLoader,         // Font data
    100.0f,            // X position
    100.0f,            // Y position
    1.0f,              // X scale
    1.0f               // Y scale
);
```

## Key Features
1. OpenGL buffer management
2. Texture loading (PNG/JPG)
3. Vertex attribute setup
4. Text rendering support

Used by:
- [[Entity]]
- [[UiElement]]