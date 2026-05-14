#include <iostream>
#include "gl_render_utils.hpp"

const static int WINDOW_WIDTH = 800;
const static int WINDOW_HEIGHT = 600;
const static std::string WINDOW_TITLE = "Code Encapsulation";

struct Vertex {
    float x, y, z; // position
    float h, v;    // texture coordinates
};

Vertex cube_vertices[] = {
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},
    {0.5f, -0.5f, -0.5f, 1.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 0.0f},

    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},
    {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 1.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 1.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},

    {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f},
    {-0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, 1.0f, 0.0f},

    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 0.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},

    {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, -0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
    {0.5f, -0.5f, 0.5f, 1.0f, 0.0f},
    {-0.5f, -0.5f, 0.5f, 0.0f, 0.0f},
    {-0.5f, -0.5f, -0.5f, 0.0f, 1.0f},

    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f},
    {0.5f, 0.5f, -0.5f, 1.0f, 1.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},
    {0.5f, 0.5f, 0.5f, 1.0f, 0.0f},
    {-0.5f, 0.5f, 0.5f, 0.0f, 0.0f},
    {-0.5f, 0.5f, -0.5f, 0.0f, 1.0f}};

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

    // main codes
    {
        // open depth testing for 3D rendering
        glEnable(GL_DEPTH_TEST);

        Shader shader(SHADER_DIR "code_encapsulation.vert", SHADER_DIR "code_encapsulation.frag");

        VertexArray vertex_array;
        VertexBuffer vertex_buffer(cube_vertices, sizeof(cube_vertices));

        VertexBufferLayout vertex_buffer_layout;
        vertex_buffer_layout.push<float>(3); // position
        vertex_buffer_layout.push<float>(2); // texture coordinates

        vertex_array.addBuffer(vertex_buffer, vertex_buffer_layout);

        Texture texture1(TEXTURE_DIR "container.jpg");
        Texture texture2(TEXTURE_DIR "awesomeface.png");

        shader.use();
        shader.setUniform1i("texture_1", 0);
        shader.setUniform1i("texture_2", 1);

        while (!glfwWindowShouldClose(window)) {
            // per-frame time logic
            camera_controller.updateFrameTime();

            // handle keyboard and mouse input
            processInput(window, camera_controller);

            // clear the color and depth buffers
            glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // bind textures
            texture1.bind(0);
            texture2.bind(1);

            // activate shader
            shader.use();

            // create transformations
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
            view = camera_controller.camera.getViewMatrix();
            projection = glm::perspective(glm::radians(camera_controller.camera.fov), getDynamicAspect(window), 0.1f, 100.0f);

            // pass transformation matrices to the shader
            shader.setUniformMat4fv("model", model);
            shader.setUniformMat4fv("view", view);
            shader.setUniformMat4fv("projection", projection);

            // render the cube
            vertex_array.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);

            // glfw: swap buffers and poll IO events
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
