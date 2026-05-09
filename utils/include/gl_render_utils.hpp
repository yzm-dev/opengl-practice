#pragma once

#include "gl_get_error/gl_get_error.hpp"

#include "gl_renderer/camera.hpp"
#include "gl_renderer/index_buffer.hpp"
#include "gl_renderer/shader.hpp"
#include "gl_renderer/texture.hpp"
#include "gl_renderer/vertex_array.hpp"
#include "gl_renderer/vertex_buffer_layout.hpp"
#include "gl_renderer/vertex_buffer.hpp"
#include "gl_renderer/renderer.hpp"

static void framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height)
{
    GLCall(glViewport(0, 0, width, height));
}

static void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
