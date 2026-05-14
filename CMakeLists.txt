cmake_minimum_required(VERSION 3.20)
project(OpenGL-Practice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# macOS/Homebrew：查询 Homebrew 安装的依赖。
if(APPLE)
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE HOMEBREW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(HOMEBREW_PREFIX)
        foreach(PACKAGE_NAME glfw glm assimp freetype)
            list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}/opt/${PACKAGE_NAME}")
        endforeach()
    endif()
endif()

# Windows 手动指定库：
# 1. 通过 CMAKE_PREFIX_PATH 指定依赖路径。
# 2. 通过 vcpkg 安装依赖，并设置 CMAKE_TOOLCHAIN_FILE 指向 vcpkg 的工具链文件。
find_package(glfw3 3.3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Freetype REQUIRED)

add_subdirectory(third_party/stb)
add_subdirectory(third_party/glad)
add_subdirectory(third_party/imgui)
add_subdirectory(utils)

function(add_opengl_executable TARGET_NAME)
    add_executable(${TARGET_NAME} ${ARGN})
    target_link_libraries(${TARGET_NAME} PRIVATE
        gl_renderer_utils
        imgui
    )

    if(APPLE)
        target_link_libraries(${TARGET_NAME} PRIVATE "-framework OpenGL")
    elseif(UNIX)
        target_link_libraries(${TARGET_NAME} PRIVATE GL)
    endif()

    # 将子项目下的路径注入为编译期宏，运行时从文件加载纹理和着色器
    target_compile_definitions(${TARGET_NAME} PRIVATE
        TEXTURE_DIR="${PROJECT_SOURCE_DIR}/resources/textures/"
        FONT_DIR="${PROJECT_SOURCE_DIR}/resources/fonts/"
        SHADER_DIR="${PROJECT_SOURCE_DIR}/${TARGET_NAME}/"
    )
endfunction()

# examples
add_opengl_executable(imgui_examples examples/imgui_demo.cpp)

# projects
add_opengl_executable(1.draw_triangle 1.draw_triangle/1.draw_triangle.cpp)
add_opengl_executable(2.texture_display 2.texture_display/2.texture_display.cpp)
add_opengl_executable(3.transformations 3.transformations/3.transformations.cpp)
add_opengl_executable(4.code_encapsulation 4.code_encapsulation/code_encapsulation.cpp)
add_opengl_executable(5.coordinate_systems 5.coordinate_systems/coordinate_systems.cpp)
add_opengl_executable(6.light_material 6.light_material/light_material.cpp)
add_opengl_executable(7.imgui_demo 7.imgui_demo/imgui_demo.cpp)
