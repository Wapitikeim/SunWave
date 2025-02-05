## Core Purpose
Renders an infinite grid using a shader-based approach with single quad and fragment shader math.

## Implementation Steps
1. Initialize quad mesh
2. Setup OpenGL buffers 
3. Configure blending
4. Use special grid shader
5. Draw with scale factor

## Example Usage
```cpp
// Create infinite grid
auto gridShader = ShaderContainer("grid.vert", "grid.frag");
auto grid = InfiniteGrid(std::move(gridShader));

// Draw with camera matrices
grid.drawGrid(1.0f);  // 1.0 = grid cell size

// Draw with custom view/projection
grid.drawGrid(
    viewMatrix,
    projectionMatrix,
    2.0f  // larger grid cells
);
```

## Key Features
6. Single quad rendering
7. Fragment shader grid calculation
8. Adjustable scale
9. Alpha blending
10. Camera integration

Used by:
- [[GameEnvironment]]
- L[[UiManager]]

Note: Requires specific grid shader for line pattern generation