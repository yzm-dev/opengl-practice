#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

static const int WINDOW_WIDTH = 800;
static const int WINDOW_HEIGHT = 600;

static std::string read_file(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Cannot open file: " << path << '\n';
        return {};
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

static void framebuffer_size_callback(GLFWwindow * /*window*/, int width, int height)
{
    glViewport(0, 0, width, height);
}

static void process_input(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static GLuint compile_shader(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
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

static GLuint create_program(const char *vert_src, const char *frag_src)
{
    GLuint vert = compile_shader(GL_VERTEX_SHADER, vert_src);
    GLuint frag = compile_shader(GL_FRAGMENT_SHADER, frag_src);
    if (!vert || !frag)
        return 0;

    GLuint program = glCreateProgram();
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
        program = 0;
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialise GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // MacOS 必须设置这个选项
#endif

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          "Draw Triangle", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::cerr << "Failed to initialise GLAD\n";
        glfwTerminate();
        return -1;
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n';

    // SHADER_DIR 在 CMake 里统一配置，指向当前项目 shaders/ 目录
    std::string vert_src = read_file(SHADER_DIR "triangle.vert");
    std::string frag_src = read_file(SHADER_DIR "triangle.frag");
    if (vert_src.empty() || frag_src.empty())
    {
        glfwTerminate();
        return -1;
    }

    GLuint shader_program = create_program(vert_src.c_str(), frag_src.c_str());
    if (!shader_program)
    {
        glfwTerminate();
        return -1;
    }

    // 三个顶点，NDC 坐标，逆时针排列
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, // 左下
        0.5f, -0.5f, 0.0f,  // 右下
        0.0f, 0.5f, 0.0f    // 顶部
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 属性 0：3 个 float，步长 3*float，偏移 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          reinterpret_cast<void *>(0));
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        process_input(window);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shader_program);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
