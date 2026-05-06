cmake_minimum_required(VERSION 3.20)
project(OpenGL-Practice)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# macOS/Homebrew：用 brew --prefix 自动找到 glfw 安装路径，无需硬编码。
# Windows用户：
# 1. 通过 CMAKE_PREFIX_PATH 指定 glfw 路径
# 2. 通过 vcpkg 安装 glfw，并设置 CMAKE_TOOLCHAIN_FILE 指向 vcpkg 的工具链文件。
if(APPLE)
    execute_process(
        COMMAND brew --prefix glfw
        OUTPUT_VARIABLE HOMEBREW_GLFW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(HOMEBREW_GLFW_PREFIX)
        list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_GLFW_PREFIX}")
    endif()
endif()

find_package(glfw3 3.3 REQUIRED)

add_subdirectory(external/libs/glad)

function(add_opengl_executable TARGET_NAME)
    add_executable(${TARGET_NAME} ${ARGN})
    target_include_directories(${TARGET_NAME} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
    )
    target_link_libraries(${TARGET_NAME} PRIVATE glad glfw)

    if(APPLE)
        target_link_libraries(${TARGET_NAME} PRIVATE "-framework OpenGL")
    elseif(UNIX)
        target_link_libraries(${TARGET_NAME} PRIVATE GL)
    endif()

    # 将子项目下的 shaders/ 路径注入为编译期宏，运行时从文件加载 shader
    target_compile_definitions(${TARGET_NAME} PRIVATE
        SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders/"
    )
endfunction()

add_subdirectory(1.DrawTriangle)