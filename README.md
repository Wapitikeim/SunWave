# SunWave Game Engine & Minigame Collection

## Overview
SunWave is a 2D physics-based minigame collection built with a custom game engine. The project features both structured gameplay through its minigame collection and an open sandbox mode for physics experimentation.

### Core Features
- Custom 2D physics engine with spatial partitioning
- Three distinct minigames with progressive difficulty
- Physics sandbox mode with real-time object manipulation
- JSON-based level serialization system
- ImGui-powered debug interface and level editor
- TTF text rendering support

### Minigames
1. **Shape?**: Test your observation skills finding specific shapes among many
2. **Go!**: Navigate through physics-based obstacles
3. **Catch!**: Test your reflexes catching falling objects
4. **Sunwave Mode**: Progressive challenge combining all minigames

### Technical Highlights
- Entity Component System architecture
- Morton encoding for spatial partitioning
- Collision detection using SAT algorithm
- High score persistence system
- Real-time physics parameter tuning

## External Sources Used

### Libraries
- [GLFW](https://www.glfw.org/) - OpenGL window/context management
- [GLM](https://github.com/g-truc/glm) - Mathematics library
- [Dear ImGui](https://github.com/ocornut/imgui) - Debug interface
- [FreeType](https://freetype.org/) - Font rendering
- [nlohmann/json](https://github.com/nlohmann/json) - JSON handling
- [GLAD](https://glad.dav1d.de/) - OpenGL loader
- [stb_image](https://github.com/nothings/stb) - Image loading library
- [soloud](https://solhsa.com/soloud/index.html) - Sound libary

### Fonts
- [Open Sans](https://fonts.google.com/specimen/Open+Sans) - Main UI font

### Infinite Grid
- [Code for infinite Grid](https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/) - Background infinite Grid 

### Images
- [Background Image for Poster](https://pixabay.com/illustrations/sun-water-boho-bohemian-color-8066051/)


### Soundtracks
- [Trailer + Menu music](https://pixabay.com/music/modern-classical-inspiring-piano-music-293598/)
- [Sunwave mode](https://suno.com/song/83f0a130-9eb4-4725-b60e-b6b9da770944)
- [Go!](https://pixabay.com/music/video-games-chiptune-grooving-142242/)
- [Catch!](https://pixabay.com/music/video-games-pixel-dreams-259187/)
- [Shape?](https://pixabay.com/music/upbeat-chill-house-nightlight-251695/)


## Installation Guide / Requirements

### Precompiled exe
- If you already have MSVC (testen with version 15/16/17) installed you can unzip the executable under /builds and start the application.

### Prerequisites
- [CMake 3.31.4+](https://github.com/Kitware/CMake/releases/download/v3.31.4/cmake-3.31.4-windows-x86_64.msi)
- Microsoft Visual Studio Compiler (MSVC - tested with version 15 16 17) [Visual Studio 2019](https://aka.ms/vs/16/release/vs_community.exe)

### Visual Studio Setup
During Visual Studio installation, ensure these workloads are selected:
- "Desktop development with C++"
- "Linux and embedded development with C++"

### Build Instructions (Windows only, on linux execute the cmake commands found in install.ps1)
1. Install prerequisites above
2. Open PowerShell as Administrator and run:
```powershell
Set-ExecutionPolicy unrestricted
```
3. Navigate to project root and execute:
```powershell
    .\install.ps1
```
4. If the cmake compiler successfully runs you can find the executable under

/code/out/build/runtime/Debug/Sunwave.exe


5.(optional) you can edit the contents of install.ps1 to match your installed msvc compiler version edit the -G "Visual Studio 16 2019" to e.g. "Visual Studio 17 2022" or "Visual Studio 15 2017" 

## Repository Usage Guides

```
RepositoryRoot/
    ├── README.md           // This should reflect your project 
    │                       //  accurately, so always merge infor- 
    │                       //  mation from your concept paper 
    │                       //  with the readme
    ├── builds/             // Archives (.zip) of built executables of your projects
    │                       //  including (non-standard) dependencies
    ├── code/
    │   ├── engine/         // Place your project folder(s) here
    │   ├── my-game-1/      // No un-used folders, no "archived" folders
    │   ├── CMakeLists.txt  // e.g. if using CMake, this can be your project root
    │   └── ...
    ├── documentation/      // GL2/3 - Each project requires FULL documentation  
    │                       //   i.e. API Docs, Handbook, Dev Docs
    ├── poster/             // PDF of your Poster(s)
    ├── report/             // PDF
    └── trailer/            // .mp4 (final trailer, no raw material)
```
