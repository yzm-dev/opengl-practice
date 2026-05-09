#include <iostream>

#include "gl_get_error/gl_get_error.hpp"
#include "gl_renderer/index_buffer.hpp"

namespace
{
unsigned int get_index_type_size(unsigned int type)
{
    switch (type)
    {
    case GL_UNSIGNED_INT:
        return sizeof(unsigned int);
    case GL_UNSIGNED_SHORT:
        return sizeof(unsigned short);
    case GL_UNSIGNED_BYTE:
        return sizeof(unsigned char);
    default:
        return 0;
    }
}
} // namespace

IndexBuffer::IndexBuffer(const void *data, unsigned int count)
    : IndexBuffer(count, GL_UNSIGNED_INT, data, GL_STATIC_DRAW)
{
}

IndexBuffer::IndexBuffer(unsigned int count, unsigned int type, const void *data, unsigned int usage)
    : renderer_id_(0), count_(count), type_(type)
{
    unsigned int type_size = get_index_type_size(type_);
    if (type_size == 0)
    {
        std::cerr << "Unsupported index buffer type: " << type_ << std::endl;
        count_ = 0;
        return;
    }

    GLCall(glGenBuffers(1, &renderer_id_));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count_ * type_size, data, usage));
}

IndexBuffer::~IndexBuffer()
{
    GLCall(glDeleteBuffers(1, &renderer_id_));
}

IndexBuffer::IndexBuffer(IndexBuffer &&other) noexcept
    : renderer_id_(other.renderer_id_),
      count_(other.count_),
      type_(other.type_)
{
    other.renderer_id_ = 0;
    other.count_ = 0;
}

IndexBuffer &IndexBuffer::operator=(IndexBuffer &&other) noexcept
{
    if (this != &other)
    {
        GLCall(glDeleteBuffers(1, &renderer_id_));
        renderer_id_ = other.renderer_id_;
        count_ = other.count_;
        type_ = other.type_;
        other.renderer_id_ = 0;
        other.count_ = 0;
    }
    return *this;
}

void IndexBuffer::bindIndexBuffer() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id_));
}

void IndexBuffer::unbindIndexBuffer() const
{
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::bind() const
{
    bindIndexBuffer();
}

void IndexBuffer::unbind() const
{
    unbindIndexBuffer();
}
