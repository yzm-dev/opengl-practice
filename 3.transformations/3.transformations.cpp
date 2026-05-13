#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "stb_image.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

const static int WINDOW_WIDTH = 800;
const static int WINDOW_HEIGHT = 600;

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

static int getTextureFormat(int nr_channels)
{
    if (nr_channels == 4)
        return GL_RGBA;
    if (nr_channels == 3)
        return GL_RGB;
    if (nr_channels == 1)
        return GL_RED;

    return 0;
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

    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Transformations", nullptr, nullptr);
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

    // 开启深度测试。GLAD 初始化后才能调用 OpenGL 函数。
    glEnable(GL_DEPTH_TEST);

    // SHADER_DIR 在 CMake 里统一配置，由 target_compile_definitions 传入
    std::string vert_src = readFile2String(SHADER_DIR "transformations.vert");
    std::string frag_src = readFile2String(SHADER_DIR "transformations.frag");
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

    unsigned int vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

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

    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, x)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void *>(offsetof(Vertex, h)));

    std::string texture_path_1 = TEXTURE_DIR "container.jpg";
    std::string texture_path_2 = TEXTURE_DIR "awesomeface.png";

    // texture 1
    int width, height, nr_channels;
    stbi_set_flip_vertically_on_load(true); // OpenGL 的纹理坐标系原点在左下角，而图片的原点通常在左上角，所以需要翻转图片
    unsigned char *texture_data = stbi_load(texture_path_1.c_str(), &width, &height, &nr_channels, 0);
    if (!texture_data)
    {
        std::cerr << "Failed to load texture: " << texture_path_1 << '\n';
        stbi_image_free(texture_data);
        glfwTerminate();
        return -1;
    }

    unsigned int texture_id_1;
    glGenTextures(1, &texture_id_1);
    glBindTexture(GL_TEXTURE_2D, texture_id_1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int format = getTextureFormat(nr_channels);
    if (format == 0)
    {
        std::cerr << "Unsupported texture channel count: " << nr_channels << '\n';
        stbi_image_free(texture_data);
        glfwTerminate();
        return -1;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(texture_data);

    // texture 2
    unsigned int texture_id_2;
    glGenTextures(1, &texture_id_2);
    glBindTexture(GL_TEXTURE_2D, texture_id_2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    texture_data = stbi_load(texture_path_2.c_str(), &width, &height, &nr_channels, 0);
    if (!texture_data)
    {
        std::cerr << "Failed to load texture: " << texture_path_2 << '\n';
        stbi_image_free(texture_data);
        glfwTerminate();
        return -1;
    }

    format = getTextureFormat(nr_channels);
    if (format == 0)
    {
        std::cerr << "Unsupported texture channel count: " << nr_channels << '\n';
        stbi_image_free(texture_data);
        glfwTerminate();
        return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, texture_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(texture_data);

    glUseProgram(shader_program);
    glUniform1i(glGetUniformLocation(shader_program, "texture_1"), 0);
    glUniform1i(glGetUniformLocation(shader_program, "texture_2"), 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind texture on corresponding texture unit
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id_1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture_id_2);

        // activate shader
        glUseProgram(shader_program);

        // create transformations
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.5f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), static_cast<float>(WINDOW_WIDTH) / WINDOW_HEIGHT, 0.1f, 100.0f);

        // retrieve the matrix uniform locations
        unsigned int model_loc = glGetUniformLocation(shader_program, "model");
        unsigned int view_loc = glGetUniformLocation(shader_program, "view");
        unsigned int projection_loc = glGetUniformLocation(shader_program, "projection");

        // pass the matrices to the shader
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
