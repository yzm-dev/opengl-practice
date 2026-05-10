#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "stb_image.h"

const static int WINDOW_WIDTH = 800;
const static int WINDOW_HEIGHT = 800;

void framebufferSizeCallback(GLFWwindow * /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static std::string readFile2String(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Cannot open file: " << path << '\n';
        return {};
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    return oss.str();
}

static unsigned int compileShader(GLenum type, const char *src)
{
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "Shader compile error:\n"
                  << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

static unsigned int createProgram(const char *vert_src, const char *frag_src)
{
    unsigned int vert = compileShader(GL_VERTEX_SHADER, vert_src);
    unsigned int frag = compileShader(GL_FRAGMENT_SHADER, frag_src);
    if (!vert || !frag)
        return 0;
    unsigned int program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(program, sizeof(log), nullptr, log);
        std::cerr << "Program link error:\n"
                  << log << '\n';
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

struct Vertex
{
    float x, y, z; // position
    float h, v;    // texture coordinates
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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS 必须设置这个选项
#endif

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Texture Display", nullptr, nullptr);
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
        glfwTerminate();
        return -1;
    }
    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';

    // SHADER_DIR 在 CMake 里统一配置，由 target_compile_definitions 传入
    std::string vert_src = readFile2String(SHADER_DIR "texture.vert");
    std::string frag_src = readFile2String(SHADER_DIR "texture.frag");
    if (vert_src.empty() || frag_src.empty())
    {
        glfwTerminate();
        return -1;
    }

    unsigned int shader_program = createProgram(vert_src.c_str(), frag_src.c_str());
    if (!shader_program)
    {
        glfwTerminate();
        return -1;
    }

    unsigned int vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

    Vertex vertices[] = {
        {-0.5f, -0.5f, 0.0f, 0.0f, 0.0f}, // 左下
        {0.5f, -0.5f, 0.0f, 1.0f, 0.0f},  // 右下
        {0.5f, 0.5f, 0.0f, 1.0f, 1.0f},   // 右上
        {-0.5f, 0.5f, 0.0f, 0.0f, 1.0f}   // 左上
    };

    unsigned int indices[] = {
        0, 1, 2, // 第一个三角形
        2, 3, 0  // 第二个三角形
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, h)));

    // std::string texture_path = TEXTURE_DIR "awesomeface.png";
    std::string texture_path = TEXTURE_DIR "container2.png";

    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *texture_data = stbi_load(texture_path.c_str(), &width, &height, &nr_channels, 0);
    if (!texture_data)
    {
        std::cerr << "Failed to load texture: " << texture_path << '\n';
        stbi_image_free(texture_data);
        glfwTerminate();
        return -1;
    }
    unsigned int texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int format = GL_RGBA;
    if (nr_channels == 4)
        format = GL_RGBA;
    else if (nr_channels == 3)
        format = GL_RGB;
    else if (nr_channels == 1)
        format = GL_RED;
    else
    {
        std::cerr << "Unsupported texture channel count: " << nr_channels << '\n';
        stbi_image_free(texture_data);
        glfwTerminate();
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (texture_data)
        stbi_image_free(texture_data);

    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "out_texture"), 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 2 * 3, GL_UNSIGNED_INT, nullptr);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
