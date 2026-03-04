cmake_minimum_required(VERSION 3.20)
project(MyEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(index src/main.cpp)

#external stuff (Vendors?)
target_include_directories(index PRIVATE vendor)

# Emscripten-specific flags
if(EMSCRIPTEN)
    set_target_properties(index PROPERTIES SUFFIX ".html")
    target_link_options(index PRIVATE
        -sUSE_WEBGL2=1               # WebGL 2
        -sFULL_ES3=1                 # OpenGL ES 3.0
        -sALLOW_MEMORY_GROWTH=1      # Dynamic memory
        --shell-file ${CMAKE_SOURCE_DIR}/shell.html  # Custom HTML shell
    )
endif()