add_library(gl_renderer_utils STATIC
    src/gl_renderer/camera.cpp
    src/gl_renderer/index_buffer.cpp
    src/gl_renderer/renderer.cpp
    src/gl_renderer/shader.cpp
    src/gl_renderer/texture.cpp
    src/gl_renderer/vertex_array.cpp
    src/gl_renderer/vertex_buffer_layout.cpp
    src/gl_renderer/vertex_buffer.cpp
)

target_include_directories(gl_renderer_utils PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/third_party/stb"
)

target_link_libraries(gl_renderer_utils PUBLIC
    glad
    glfw
    glm::glm
    assimp::assimp
    Freetype::Freetype
)
