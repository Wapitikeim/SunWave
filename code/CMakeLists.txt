# CMakeList.txt: CMake-Projekt für "CMakeSandboxLaptop". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required(VERSION 3.7...3.27)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()
message([STATUS] "CMAKE VERSION: "${CMAKE_VERSION})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
#set_property(TARGET tgt PROPERTY CXX_STANDARD 20)

project(HiddenInstinct VERSION 0.3
                  DESCRIPTION "GameLab3 Project"
                  LANGUAGES CXX)


#Output Directory https://stackoverflow.com/questions/6594796/how-do-i-make-cmake-output-into-a-bin-dir
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out/build/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/out/build/runtime)

#[[
file(GLOB SOURCEFILES 
 src/*.h 
 src/*.cpp
 src/util/*.cpp
 src/util/*.h
 src/shaders/*.cpp
 src/shaders/*.h
 src/entities/*.cpp
 src/entities/*.h
 src/mesh/*.h
 src/mesh/*.cpp
 src/fieldOfShapes/*.h
 src/fieldOfShapes/*.cpp)
]]

file(GLOB SOURCEFILES src/*.cpp src/*/*.cpp)
file(GLOB HEADERFILES src/*.h src/*/*.h)

add_subdirectory(extern/glfw)
add_subdirectory(extern/glad)
add_subdirectory(extern/glm)
add_subdirectory(extern/stbimage)
add_subdirectory(extern/imgui)
add_subdirectory(extern/nhlohmann_json)
# FreeType configuration
set(FT_DISABLE_ZLIB TRUE CACHE BOOL "Disable ZLIB")
set(FT_DISABLE_BZIP2 TRUE CACHE BOOL "Disable BZIP2")
set(FT_DISABLE_PNG TRUE CACHE BOOL "Disable PNG")
set(FT_DISABLE_HARFBUZZ TRUE CACHE BOOL "Disable HarfBuzz")
set(FT_DISABLE_BROTLI TRUE CACHE BOOL "Disable Brotli")
add_subdirectory(extern/freetype)

# Cmake glfw stuff
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (HiddenInstinct ${SOURCEFILES} ${HEADERFILES})

# opengl32.lib -> included in glad
target_link_libraries(HiddenInstinct PRIVATE glfw glad glm stbimage imgui nlohmann_json freetype)

# require C++ 17 compiler
target_compile_features(HiddenInstinct PRIVATE cxx_std_17)

# TODO: Fügen Sie bei Bedarf Tests hinzu, und installieren Sie Ziele.
