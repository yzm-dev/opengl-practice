#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "gl_get_error/gl_get_error.hpp"
#include "gl_renderer/shader.hpp"
#include "glm/gtc/type_ptr.hpp"

Shader::Shader(const std::string &vertex_file_path, const std::string &fragment_file_path) : renderer_id_(0),
                                                                                             vertex_file_path_(vertex_file_path),
                                                                                             fragment_file_path_(fragment_file_path) {
    std::string vertex_shader_source = readShaderSourceFromFile(vertex_file_path_);
    std::string fragment_shader_source = readShaderSourceFromFile(fragment_file_path_);
    if (vertex_shader_source.empty() || fragment_shader_source.empty()) {
        std::cerr << "Failed to read shader source from files." << std::endl;
        return;
    }
    renderer_id_ = createShader(vertex_shader_source, fragment_shader_source);
    if (renderer_id_ == 0) {
        std::cerr << "Failed to create shader program." << std::endl;
    }
}

Shader::~Shader() {
    GLCall(glDeleteProgram(renderer_id_));
}

Shader::Shader(Shader &&other) noexcept
    : renderer_id_(other.renderer_id_),
      vertex_file_path_(std::move(other.vertex_file_path_)),
      fragment_file_path_(std::move(other.fragment_file_path_)),
      uniform_location_cache_(std::move(other.uniform_location_cache_)) {
    other.renderer_id_ = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept {
    if (this != &other) {
        GLCall(glDeleteProgram(renderer_id_));
        renderer_id_ = other.renderer_id_;
        vertex_file_path_ = std::move(other.vertex_file_path_);
        fragment_file_path_ = std::move(other.fragment_file_path_);
        uniform_location_cache_ = std::move(other.uniform_location_cache_);
        other.renderer_id_ = 0;
    }
    return *this;
}

void Shader::use() const {
    GLCall(glUseProgram(renderer_id_));
}

void Shader::unuse() const {
    GLCall(glUseProgram(0));
}

void Shader::setUniform1i(const std::string &name, int value) {
    GLCall(glUniform1i(getUniformLocation(name), value));
}

void Shader::setUniform1f(const std::string &name, float value) {
    GLCall(glUniform1f(getUniformLocation(name), value));
}

void Shader::setUniform3i(const std::string &name, int value_0, int value_1, int value_2) {
    GLCall(glUniform3i(getUniformLocation(name), value_0, value_1, value_2));
}

void Shader::setUniform3f(const std::string &name, float value_0, float value_1, float value_2) {
    GLCall(glUniform3f(getUniformLocation(name), value_0, value_1, value_2));
}

void Shader::setUniform3fv(const std::string &name, const glm::vec3 &vector) {
    GLCall(glUniform3fv(getUniformLocation(name), 1, glm::value_ptr(vector)));
}

void Shader::setUniform4f(const std::string &name, float value_0, float value_1, float value_2, float value_3) {
    GLCall(glUniform4f(getUniformLocation(name), value_0, value_1, value_2, value_3));
}

void Shader::setUniformMat4fv(const std::string &name, const glm::mat4 &matrix) {
    GLCall(glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)));
}

int Shader::getUniformLocation(const std::string &name) {
    auto cached_location = uniform_location_cache_.find(name);
    if (cached_location != uniform_location_cache_.end()) {
        return cached_location->second;
    }

    if (renderer_id_ == 0) {
        return -1;
    }

    int location = -1;
    GLCall(location = glGetUniformLocation(renderer_id_, name.c_str()));
    if (location == -1) {
        std::cout << "Warning: uniform " << name << " does not exist!" << std::endl;
        return -1;
    }

    uniform_location_cache_[name] = location;
    return location;
}

std::string Shader::readShaderSourceFromFile(const std::string &file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Cannot open shader file: " << file_path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

unsigned int Shader::compileShader(GLenum shader_type, const std::string &shader_source) {
    unsigned int shader = 0;
    GLCall(shader = glCreateShader(shader_type));
    const char *src = shader_source.c_str();
    GLCall(glShaderSource(shader, 1, &src, nullptr));
    GLCall(glCompileShader(shader));

    int success;
    GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &success));
    if (!success) {
        char info_log[512];
        GLCall(glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log));
        std::cerr << "Shader compile error:\n"
                  << info_log << std::endl;
        GLCall(glDeleteShader(shader));
        return 0;
    }
    return shader;
}

unsigned int Shader::createShader(const std::string &vertex_shader_source, const std::string &fragment_shader_source) {
    unsigned int vertex_shader = compileShader(GL_VERTEX_SHADER, vertex_shader_source);
    unsigned int fragment_shader = compileShader(GL_FRAGMENT_SHADER, fragment_shader_source);
    if (vertex_shader == 0 || fragment_shader == 0) {
        GLCall(glDeleteShader(vertex_shader));
        GLCall(glDeleteShader(fragment_shader));
        return 0;
    }

    unsigned int shader_program = 0;
    GLCall(shader_program = glCreateProgram());
    GLCall(glAttachShader(shader_program, vertex_shader));
    GLCall(glAttachShader(shader_program, fragment_shader));
    GLCall(glLinkProgram(shader_program));

    int success;
    GLCall(glGetProgramiv(shader_program, GL_LINK_STATUS, &success));
    if (!success) {
        char info_log[512];
        GLCall(glGetProgramInfoLog(shader_program, sizeof(info_log), nullptr, info_log));
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                  << info_log << std::endl;
        GLCall(glDeleteProgram(shader_program));
        shader_program = 0;
    }

    GLCall(glDeleteShader(vertex_shader));
    GLCall(glDeleteShader(fragment_shader));
    return shader_program;
}
