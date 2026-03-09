cmake_minimum_required(VERSION 3.20)
project(MyEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(index src/main.cpp
                src/renderer.cpp
                src/mesh.cpp
                src/shadowFramebuffer.cpp
                src/meshImporter.cpp
                src/instancedMesh.cpp
                src/colorFramebuffer.cpp
                src/fullscreenQuad.cpp)

#external stuff (Vendors?)
target_include_directories(index PRIVATE vendor)

set_property(TARGET index PROPERTY LINK_DEPENDS
    ${CMAKE_SOURCE_DIR}/shaders/basic.vert
    ${CMAKE_SOURCE_DIR}/shaders/basic.frag
    ${CMAKE_SOURCE_DIR}/shaders/instanced.vert
    ${CMAKE_SOURCE_DIR}/shaders/instanced_shadow.vert
)

# Emscripten-specific flags
if(EMSCRIPTEN)
    set_target_properties(index PROPERTIES SUFFIX ".html")
    target_link_options(index PRIVATE
        -sUSE_WEBGL2=1               # WebGL 2
        -sFULL_ES3=1                 # OpenGL ES 3.0
        -sALLOW_MEMORY_GROWTH=1      # Dynamic memory
    "SHELL:--shell-file ${CMAKE_SOURCE_DIR}/shell.html"
    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/shaders\@/shaders"
    "SHELL:--preload-file ${CMAKE_SOURCE_DIR}/models\@/models"
    )
endif()