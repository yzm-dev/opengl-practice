#include "gl_get_error/gl_get_error.hpp"
#include "gl_renderer/vertex_buffer.hpp"
#include "glad/glad.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size)
    : VertexBuffer(size, data, GL_STATIC_DRAW)
{
}

VertexBuffer::VertexBuffer(unsigned int size, const void *data, unsigned int usage)
    : renderer_id_(0)
{
    GLCall(glGenBuffers(1, &renderer_id_));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, usage));
}

VertexBuffer::~VertexBuffer()
{
    GLCall(glDeleteBuffers(1, &renderer_id_));
}

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
    : renderer_id_(other.renderer_id_)
{
    other.renderer_id_ = 0;
}

VertexBuffer &VertexBuffer::operator=(VertexBuffer &&other) noexcept
{
    if (this != &other)
    {
        GLCall(glDeleteBuffers(1, &renderer_id_));
        renderer_id_ = other.renderer_id_;
        other.renderer_id_ = 0;
    }
    return *this;
}

void VertexBuffer::bindVertexBuffer() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, renderer_id_));
}

void VertexBuffer::unbindVertexBuffer() const
{
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::bind() const
{
    bindVertexBuffer();
}

void VertexBuffer::unbind() const
{
    unbindVertexBuffer();
}
