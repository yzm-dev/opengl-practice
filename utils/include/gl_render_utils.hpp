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

#include "imgui.h"
#include "stb_image.h"

struct CameraController {
    Camera camera;
    float delta_time = 0.0f; // time between current frame and last frame
    float last_frame = 0.0f; // time of last frame

    float last_x = 0.0f; // last mouse x position
    float last_y = 0.0f; // last mouse y position

    bool first_mouse = true; // whether this is the first mouse movement
    bool is_mouse_captured = true; // whether the mouse is currently captured by the application
    bool was_escape_pressed = false; // whether the escape key was pressed

    CameraController(int window_width, int window_height, glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 3.0f)) : camera(camera_position), last_x(static_cast<float>(window_width) / 2.0f), last_y(static_cast<float>(window_height) / 2.0f) {
    }

    void updateFrameTime() {
        float current_frame = static_cast<float>(glfwGetTime());
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
    }

    void setMouseCaptured(GLFWwindow *window, bool captured) {
        is_mouse_captured = captured;
        first_mouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
};

// calculate the aspect ratio based on the current framebuffer size
inline float getDynamicAspect(GLFWwindow *window, float fallback_aspect = 800.0f / 600.0f) {
    int framebuffer_width = 0;
    int framebuffer_height = 0;
    glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

    if (framebuffer_width <= 0 || framebuffer_height <= 0) {
        return fallback_aspect;
    }

    return static_cast<float>(framebuffer_width) / framebuffer_height;
}

inline void syncFramebufferSize(GLFWwindow *window) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    GLCall(glViewport(0, 0, width, height));
}

// callback function for window resize events
inline void framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height) {
    GLCall(glViewport(0, 0, width, height));
}

inline void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
inline void processInput(GLFWwindow *window, CameraController &camera_controller) {
    if (ImGui::GetCurrentContext()) {
        ImGuiIO &io = ImGui::GetIO();
        if (camera_controller.is_mouse_captured)
            // 相机模式：ImGui 禁用鼠标。
            io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
        else
            // UI 模式：ImGui 启用鼠标。
            io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }

    // 每帧检测 ESC 键状态变化，以实现按 ESC 键切换鼠标捕获状态或关闭窗口的功能
    bool is_escape_pressed = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    // 按下了 ESC 键，但之前没有按下过
    if (is_escape_pressed && !camera_controller.was_escape_pressed) {
        if (camera_controller.is_mouse_captured) // 鼠标在捕获状态，按 ESC 释放鼠标
            camera_controller.setMouseCaptured(window, false);
        else // 鼠标不在捕获状态，按 ESC 关闭窗口
            glfwSetWindowShouldClose(window, true);
    }
    camera_controller.was_escape_pressed = is_escape_pressed; // 更新 ESC 键状态

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && camera_controller.is_mouse_captured)
        camera_controller.camera.processKeyboard(CameraMovement::FORWARD, camera_controller.delta_time);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && camera_controller.is_mouse_captured)
        camera_controller.camera.processKeyboard(CameraMovement::BACKWARD, camera_controller.delta_time);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && camera_controller.is_mouse_captured)
        camera_controller.camera.processKeyboard(CameraMovement::LEFT, camera_controller.delta_time);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && camera_controller.is_mouse_captured)
        camera_controller.camera.processKeyboard(CameraMovement::RIGHT, camera_controller.delta_time);
}

// glfw: whenever the mouse moves, this callback is called
inline void mouseCallback(GLFWwindow *window, double xpos_in, double ypos_in) {
    auto *camera_controller = static_cast<CameraController *>(glfwGetWindowUserPointer(window));
    if (!camera_controller)
        return;
    if (!camera_controller->is_mouse_captured)
        return;

    float x_pos = static_cast<float>(xpos_in);
    float y_pos = static_cast<float>(ypos_in);
    if (camera_controller->first_mouse) {
        camera_controller->last_x = x_pos;
        camera_controller->last_y = y_pos;
        camera_controller->first_mouse = false;
    }
    float x_offset = x_pos - camera_controller->last_x;
    float y_offset = camera_controller->last_y - y_pos; // reversed since y-coordinates go from

    camera_controller->last_x = x_pos;
    camera_controller->last_y = y_pos;

    camera_controller->camera.processMouseMovement(x_offset, y_offset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
inline void scrollCallback(GLFWwindow *window, double /*x_offset*/, double y_offset) {
    auto *camera_controller = static_cast<CameraController *>(glfwGetWindowUserPointer(window));
    if (!camera_controller)
        return;
    if (!camera_controller->is_mouse_captured)
        return;
    if (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse)
        return;
    camera_controller->camera.processMouseScroll(static_cast<float>(y_offset));
}

inline void mouseButtonCallback(GLFWwindow *window, int button, int action, int /*mods*/) {
    auto *camera_controller = static_cast<CameraController *>(glfwGetWindowUserPointer(window));
    if (!camera_controller)
        return;
    if (ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse)
        return;

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !camera_controller->is_mouse_captured)
        camera_controller->setMouseCaptured(window, true);
}
