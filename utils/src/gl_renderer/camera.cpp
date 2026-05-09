#include "gl_renderer/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/trigonometric.hpp>

Camera::Camera(glm::vec3 start_position, glm::vec3 world_up_direction, float start_yaw, float start_pitch)
    : position(start_position),
      front(glm::vec3(0.0f, 0.0f, -1.0f)),
      world_up(world_up_direction),
      yaw(start_yaw),
      pitch(start_pitch),
      movement_speed(default_speed),
      mouse_sensitivity(default_sensitivity),
      fov(default_fov)
{
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::processKeyboard(CameraMovement direction, float delta_time)
{
    float velocity = movement_speed * delta_time;
    if (direction == CameraMovement::FORWARD)
        position += front * velocity;
    if (direction == CameraMovement::BACKWARD)
        position -= front * velocity;
    if (direction == CameraMovement::LEFT)
        position -= right * velocity;
    if (direction == CameraMovement::RIGHT)
        position += right * velocity;
}

void Camera::processMouseMovement(float x_offset, float y_offset, bool constrain_pitch)
{
    x_offset *= mouse_sensitivity;
    y_offset *= mouse_sensitivity;

    yaw += x_offset;
    pitch += y_offset;

    if (constrain_pitch)
    {
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float y_offset)
{
    fov -= y_offset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void Camera::updateCameraVectors()
{
    glm::vec3 new_front;
    new_front.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
    new_front.y = glm::sin(glm::radians(pitch));
    new_front.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));

    front = glm::normalize(new_front);
    right = glm::normalize(glm::cross(front, world_up));
    up = glm::normalize(glm::cross(right, front));
}
