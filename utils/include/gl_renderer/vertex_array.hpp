#pragma once

#include "index_buffer.hpp"
#include "vertex_buffer.hpp"
#include "vertex_buffer_layout.hpp"

class VertexArray
{
private:
    unsigned int renderer_id_;

public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray &) = delete;
    VertexArray &operator=(const VertexArray &) = delete;
    VertexArray(VertexArray &&other) noexcept;
    VertexArray &operator=(VertexArray &&other) noexcept;

    void bindVertexArray() const;
    void unbindVertexArray() const;
    void bind() const;
    void unbind() const;

    void addBuffer(const VertexBuffer &vertex_buffer, const VertexBufferLayout &layout);
    void addVertexBuffer(const VertexBuffer &vertex_buffer,
                         unsigned int index,
                         int count,
                         unsigned int type,
                         unsigned char normalized,
                         unsigned int stride,
                         const void *offset);
    void attachIndexBuffer(const IndexBuffer &index_buffer);
};
