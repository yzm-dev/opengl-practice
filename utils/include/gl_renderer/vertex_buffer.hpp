#pragma once

#include "glad/glad.h"

class VertexBuffer
{
private:
    unsigned int renderer_id_;

public:
    VertexBuffer(const void *data, unsigned int size);
    VertexBuffer(unsigned int size, const void *data, unsigned int usage = GL_STATIC_DRAW);
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer &) = delete;
    VertexBuffer &operator=(const VertexBuffer &) = delete;
    VertexBuffer(VertexBuffer &&other) noexcept;
    VertexBuffer &operator=(VertexBuffer &&other) noexcept;

    void bindVertexBuffer() const;
    void unbindVertexBuffer() const;
    void bind() const;
    void unbind() const;
};
