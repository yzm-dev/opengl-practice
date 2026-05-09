#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

inline constexpr float default_yaw = -90.0f;
inline constexpr float default_pitch = 0.0f;
inline constexpr float default_speed = 2.5f;
inline constexpr float default_sensitivity = 0.1f;
inline constexpr float default_fov = 45.0f;

class Camera
{
public:
    // 摄像机属性
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;
    // 欧拉角
    float yaw;
    float pitch;
    // 摄像机选项
    float movement_speed;
    float mouse_sensitivity;
    float fov;
    // 构造函数
    Camera(glm::vec3 start_position = glm::vec3(0.0f, 0.0f, 0.0f),
           glm::vec3 world_up_direction = glm::vec3(0.0f, 1.0f, 0.0f),
           float start_yaw = default_yaw,
           float start_pitch = default_pitch);
    // 返回视图矩阵
    glm::mat4 getViewMatrix();
    // 处理键盘输入
    void processKeyboard(CameraMovement direction, float delta_time);
    // 处理鼠标输入
    void processMouseMovement(float x_offset, float y_offset, bool constrain_pitch = true);
    // 处理鼠标滚轮输入
    void processMouseScroll(float y_offset);

private:
    // 更新摄像机的方向向量
    void updateCameraVectors();
};
