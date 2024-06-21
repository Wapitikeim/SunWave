# set stb_Image source files variable
file(GLOB SOURCEFILES src/*.cpp backends/imgui_impl_glfw.cpp backends/imgui_impl_opengl3.cpp)
file(GLOB HEADERFILES src/*.h backends/imgui_impl_glfw.h backends/imgui_impl_opengl3.h)

# add stb_Image as a static library
add_library(imgui STATIC ${HEADERFILES} ${SOURCEFILES})

# require C++ 17 compiler
target_compile_features(imgui PRIVATE cxx_std_17)

# specify the include directory for this static libray
target_include_directories(imgui PUBLIC src/ backends/)
