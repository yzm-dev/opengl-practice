#pragma once

#include "glad/glad.h"

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() = default;

    void clear() const;
    void drawArrays(const GLenum mode, const GLsizei count, const GLenum type, const void *indices) const;
    void drawElements(const GLenum mode, const GLsizei count, const GLenum type, const void *indices) const;
};