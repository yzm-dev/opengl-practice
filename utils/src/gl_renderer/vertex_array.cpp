#include <cstdint>

#include "gl_get_error/gl_get_error.hpp"
#include "gl_renderer/vertex_array.hpp"

VertexArray::VertexArray()
    : renderer_id_(0)
{
    GLCall(glGenVertexArrays(1, &renderer_id_));
}

VertexArray::~VertexArray()
{
    GLCall(glDeleteVertexArrays(1, &renderer_id_));
}

VertexArray::VertexArray(VertexArray &&other) noexcept
    : renderer_id_(other.renderer_id_)
{
    other.renderer_id_ = 0;
}

VertexArray &VertexArray::operator=(VertexArray &&other) noexcept
{
    if (this != &other)
    {
        GLCall(glDeleteVertexArrays(1, &renderer_id_));
        renderer_id_ = other.renderer_id_;
        other.renderer_id_ = 0;
    }
    return *this;
}

void VertexArray::bindVertexArray() const
{
    GLCall(glBindVertexArray(renderer_id_));
}

void VertexArray::unbindVertexArray() const
{
    GLCall(glBindVertexArray(0));
}

void VertexArray::bind() const
{
    bindVertexArray();
}

void VertexArray::unbind() const
{
    unbindVertexArray();
}

void VertexArray::addBuffer(const VertexBuffer &vertex_buffer, const VertexBufferLayout &layout)
{
    bindVertexArray();
    vertex_buffer.bindVertexBuffer();
    const auto &elements = layout.getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); i++)
    {
        const auto &element = elements[i];
        GLCall(glEnableVertexAttribArray(i));
        GLCall(glVertexAttribPointer(i,
                                     element.count,
                                     element.type,
                                     element.normalized,
                                     layout.getStride(),
                                     reinterpret_cast<const void *>(static_cast<std::uintptr_t>(offset))));
        offset += element.count * VertexBufferElement::getStrideOfType(element.type);
    }
}

void VertexArray::addVertexBuffer(const VertexBuffer &vertex_buffer,
                                  unsigned int index,
                                  int count,
                                  unsigned int type,
                                  unsigned char normalized,
                                  unsigned int stride,
                                  const void *offset)
{
    bindVertexArray();
    vertex_buffer.bindVertexBuffer();
    GLCall(glEnableVertexAttribArray(index));
    GLCall(glVertexAttribPointer(index, count, type, normalized, stride, offset));
}

void VertexArray::attachIndexBuffer(const IndexBuffer &index_buffer)
{
    bindVertexArray();
    index_buffer.bindIndexBuffer();
}
