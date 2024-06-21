# set stb_Image source files variable
set(HEADER_FILES *h backends/imgui_impl_glfw.h)
set(SOURCE_FILES *.cpp backends/imgui_impl_glfw.cpp)

# add stb_Image as a static library
add_library(imgui STATIC ${HEADER_FILES} ${SOURCE_FILES})

# require C++ 17 compiler
target_compile_features(imgui PRIVATE cxx_std_17)

# specify the include directory for this static libray
target_include_directories(imgui PUBLIC examples/ backends)
