## Core Purpose
Static utility class for file operations, primarily handling:
1. Path management
2. Shader loading
3. Level file discovery
4. File content extraction

## Key Operations

### Path Management
```cpp
// Trim path to working directory
std::filesystem::path path = std::filesystem::current_path();
fileReader::trimDownPathToWorkingDirectory(path);
```

### Shader Loading
```cpp
// Load shader from file
std::string shaderSource = fileReader::readShader("vertex.glsl");
```

### Level Management
```cpp
// Get all available level files
std::vector<std::string> levels = fileReader::getAllLevelFileNames();

// Get specific file path
auto path = fileReader::getPathToFileInFolder(
    "level1.json",
    "LevelConfigurations"
);
```

## Key Features
1. Path normalization
2. File content reading
3. Extension filtering
4. Directory traversal
5. Error checking

Used by:
- [[ShaderContainer]]
- [[sceneManager]]
- [[FontLoader]]

Note: All paths are relative to working directory (`/out/`)