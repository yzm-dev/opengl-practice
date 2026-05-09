#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_render_utils.hpp"

#include <cstddef>
#include <iostream>

static constexpr int WINDOW_WIDTH = 800;
static constexpr int WINDOW_HEIGHT = 600;

struct Vertex
{
    float x, y, z;
    float r, g, b;
};

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "OpenGL Utils Example", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';

    {
        Shader shader(SHADER_DIR "triangle.vert", SHADER_DIR "triangle.frag");

        Vertex vertices[] = {
            {-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f},
            {0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f},
            {0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f},
        };
        unsigned int indices[] = {0, 1, 2};

        VertexArray vao;
        VertexBuffer vertex_buffer(sizeof(vertices), vertices, GL_STATIC_DRAW);
        IndexBuffer index_buffer(3, GL_UNSIGNED_INT, indices, GL_STATIC_DRAW);

        // VertexBufferLayout layout;
        // layout.push<float>(3); // position
        // layout.push<float>(3); // color
        // vao.addBuffer(vertex_buffer, layout);

        vao.addVertexBuffer(vertex_buffer, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, x)));
        vao.addVertexBuffer(vertex_buffer, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, r)));
        vao.attachIndexBuffer(index_buffer);

        vao.unbind();
        vertex_buffer.unbind();
        index_buffer.unbind();

        Renderer renderer;

        while (!glfwWindowShouldClose(window))
        {
            processInput(window);

            renderer.clear();

            shader.bind();
            vao.bind();
            glDrawElements(GL_TRIANGLES, index_buffer.count(), index_buffer.type(), nullptr);
            // renderer.drawElements(GL_TRIANGLES, index_buffer.count(), GL_INT, nullptr); // 使用GL_INT检验GLCall错误处理
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
