## Core Purpose
Static utility class providing global access to view, projection and orthographic matrices.

## Implementation
```cpp
// Get matrices
glm::mat4 view = Camera::getCurrentCameraView();
glm::mat4 proj = Camera::getCurrentCameraProjection();
glm::mat4 ortho = Camera::getCurrentCameraOrto();

// Set matrices
Camera::setCurrentCameraView(viewMatrix);
Camera::setCurrentCameraProjection(projMatrix);
Camera::setCurrentCameraOrto(glm::ortho(0.0f, width, 0.0f, height));
```

## Key Features
1. Static access to camera matrices
2. Default orthographic setup (1920x1080)
3. Global state management

Used by:
- [[ShaderContainer]]
- [[GameEnvironment]]
- [[UiElement]]

Note: Marked as temporary solution, pending scene system implementation