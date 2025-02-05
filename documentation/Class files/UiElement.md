## Overview
Implementation of Entity class for UI/text rendering with click functionality.

## Core Features
1. Text rendering with TTF fonts  (available Fonts found under ui/Fonts/...)
2. Click event handling
3. Dynamic text sizing
4. Color customization
5. World-to-screen space conversion

## Example Usage
```cpp
// Create clickable text button
auto button = std::make_unique<UiElement>(
    "startButton",               // name
    glm::vec3(0.0f),            // position
    glm::vec3(1.0f),            // scale
    0.0f,                       // rotation
    "Click Me!",                // text
    "OpenSans.ttf",                // font
    32                          // font size
);

// Add click behavior
button->setOnClick([]() {
    // Click handler
    std::cout << "Button clicked!" << std::endl;
});

// Customize appearance
glm::vec4 color(1.0f, 0.0f, 0.0f, 1.0f);  // Red
button->setTextColor(color);

// Add collision for click detection
button->addComponent(std::make_unique<PhysicsCollider>(button.get(), true));
```

Key features:
- TTF font rendering
- Automatic text size calculation
- Screen space positioning
- Click event callbacks
- Color customization