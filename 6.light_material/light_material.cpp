#include "gl_render_utils.hpp"
#include <iostream>

static int WINDOW_WIDTH = 800;
static int WINDOW_HEIGHT = 600;
const static std::string WINDOW_TITLE = "Light Material";

struct Vertex {
    float x, y, z;    // positionz
    float nx, ny, nz; // normal
};

Vertex cube_vertices[] = {
    // positions          // normals
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f},

    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f},

    {-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f},

    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    {0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    {0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f},
    {0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f},

    {-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f},

    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f}};

// light position
glm::vec3 light_pos(1.2f, 1.0f, 1.5f);

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // create camera controller and set it as the user pointer of the window
    CameraController camera_controller(WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetWindowUserPointer(window, &camera_controller);
    camera_controller.setMouseCaptured(window, true);

    // set mouse scroll and mouse button callbacks after setting the user pointer
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        glfwTerminate();
        return -1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';

    Renderer renderer;

    // main codes
    {
        // open depth testing for 3D rendering
        glEnable(GL_DEPTH_TEST);

        Shader cube_shader(SHADER_DIR "cube.vert", SHADER_DIR "cube.frag");
        Shader light_shader(SHADER_DIR "light.vert", SHADER_DIR "light.frag");

        VertexArray light_vao, cube_vao;
        VertexBuffer vbo(cube_vertices, sizeof(cube_vertices));

        VertexBufferLayout vertex_buffer_layout;
        vertex_buffer_layout.push<float>(3); // position
        vertex_buffer_layout.push<float>(3); // normal

        light_vao.addBuffer(vbo, vertex_buffer_layout);
        cube_vao.addBuffer(vbo, vertex_buffer_layout);

        while (!glfwWindowShouldClose(window)) {
            // per-frame time logic
            camera_controller.updateFrameTime();

            // handle keyboard and mouse input
            processInput(window, camera_controller);

            // clear the color and depth buffers
            glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // view/projection transformations
            glm::mat4 view = camera_controller.camera.getViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera_controller.camera.fov), getDynamicAspect(window), 0.1f, 100.0f);
            // light properties
            float time = static_cast<float>(glfwGetTime());
            glm::vec3 light_color;
            light_color.x = sin(time * 2.0f) * 0.5f + 0.5f;
            light_color.y = sin(time * 0.7f) * 0.5f + 0.5f;
            light_color.z = sin(time * 1.3f) * 0.5f + 0.5f;
            glm::vec3 diffuse_color = light_color * glm::vec3(0.5f);   // decrease the influence
            glm::vec3 ambient_color = diffuse_color * glm::vec3(0.2f); // low influence

            cube_shader.use();
            cube_shader.setUniform3fv("light.position", light_pos);
            cube_shader.setUniform3f("view_pos", camera_controller.camera.position.x, camera_controller.camera.position.y, camera_controller.camera.position.z);
            cube_shader.setUniform3fv("light.ambient", ambient_color);
            cube_shader.setUniform3fv("light.diffuse", diffuse_color);
            cube_shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
            // material properties
            cube_shader.setUniform3f("material.ambient", 1.0f, 0.5f, 0.31f);
            cube_shader.setUniform3f("material.diffuse", 1.0f, 0.5f, 0.31f);
            cube_shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect
            cube_shader.setUniform1f("material.shininess", 24.0f);

            // world transformation
            glm::mat4 model = glm::mat4(1.0f);
            cube_shader.setUniformMat4fv("model", model);
            cube_shader.setUniformMat4fv("view", view);
            cube_shader.setUniformMat4fv("projection", projection);

            cube_vao.bind();
            renderer.drawArrays(GL_TRIANGLES, 36, GL_FLOAT);

            light_shader.use();
            model = glm::mat4(1.0f);
            model = glm::translate(model, light_pos);
            model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube for the light source
            light_shader.setUniformMat4fv("model", model);
            light_shader.setUniformMat4fv("view", view);
            light_shader.setUniformMat4fv("projection", projection);
            light_vao.bind();
            renderer.drawArrays(GL_TRIANGLES, 36, GL_FLOAT);

            // glfw: swap buffers and poll IO events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
