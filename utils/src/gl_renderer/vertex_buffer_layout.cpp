#include <cassert>

#include "gl_renderer/vertex_buffer_layout.hpp"
#include "glad/glad.h"

unsigned int VertexBufferElement::getStrideOfType(unsigned int type)
{
    switch (type)
    {
    case GL_FLOAT:
        return sizeof(float);
    case GL_UNSIGNED_INT:
        return sizeof(unsigned int);
    case GL_UNSIGNED_BYTE:
        return sizeof(unsigned char);
    }
    assert(false);
    return 0;
}

VertexBufferLayout::VertexBufferLayout()
    : stride_(0)
{
}

VertexBufferLayout::~VertexBufferLayout()
{
}

template <>
void VertexBufferLayout::push<float>(unsigned int count)
{
    elements_.push_back({GL_FLOAT, count, GL_FALSE});
    stride_ += count * VertexBufferElement::getStrideOfType(GL_FLOAT);
}

template <>
void VertexBufferLayout::push<unsigned int>(unsigned int count)
{
    elements_.push_back({GL_UNSIGNED_INT, count, GL_FALSE});
    stride_ += count * VertexBufferElement::getStrideOfType(GL_UNSIGNED_INT);
}

template <>
void VertexBufferLayout::push<unsigned char>(unsigned int count)
{
    elements_.push_back({GL_UNSIGNED_BYTE, count, GL_TRUE});
    stride_ += count * VertexBufferElement::getStrideOfType(GL_UNSIGNED_BYTE);
}

const std::vector<VertexBufferElement> &VertexBufferLayout::getElements() const
{
    return elements_;
}

unsigned int VertexBufferLayout::getStride() const
{
    return stride_;
}
