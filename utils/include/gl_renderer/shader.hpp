#pragma once

#include <string>
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"

class Shader {
private:
    unsigned int renderer_id_;
    std::string vertex_file_path_;
    std::string fragment_file_path_;
    std::unordered_map<std::string, int> uniform_location_cache_;

public:
    Shader(const std::string &vertex_file_path,
           const std::string &fragment_file_path);
    ~Shader();

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;
    Shader(Shader &&other) noexcept;
    Shader &operator=(Shader &&other) noexcept;

    void use() const;
    void unuse() const;

    std::string readShaderSourceFromFile(const std::string &file_path);
    void setUniform1i(const std::string &name, int value);
    void setUniform1f(const std::string &name, float value);
    void setUniform3i(const std::string &name, int value_0, int value_1, int value_2);
    void setUniform3f(const std::string &name, float value_0, float value_1, float value_2);
    void setUniform3fv(const std::string &name, const glm::vec3 &vector);
    void setUniform4f(const std::string &name, float value_0, float value_1, float value_2, float value_3);
    void setUniformMat4fv(const std::string &name, const glm::mat4 &matrix);

private:
    int getUniformLocation(const std::string &name);

    unsigned int compileShader(GLenum shader_type,
                               const std::string &shader_source);
    unsigned int createShader(const std::string &vertex_shader_source,
                              const std::string &fragment_shader_source);
};
