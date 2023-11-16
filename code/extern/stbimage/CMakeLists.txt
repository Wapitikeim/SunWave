# set stb_Image source files variable
set(HEADER_FILES include/stb_image.h)
set(SOURCE_FILES stb_image.cpp)

# add stb_Image as a static library
add_library(stbimage STATIC ${HEADER_FILES} ${SOURCE_FILES})

# require C++ 17 compiler
target_compile_features(stbimage PRIVATE cxx_std_17)

# specify the include directory for this static libray
target_include_directories(stbimage PUBLIC include/)
