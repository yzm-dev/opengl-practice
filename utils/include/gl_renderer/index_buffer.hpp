#pragma once

#include "glad/glad.h"

class IndexBuffer
{
private:
    unsigned int renderer_id_;
    unsigned int count_;
    unsigned int type_;

public:
    IndexBuffer(const void *data, unsigned int count);
    IndexBuffer(unsigned int count, unsigned int type, const void *data, unsigned int usage = GL_STATIC_DRAW);
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer &) = delete;
    IndexBuffer &operator=(const IndexBuffer &) = delete;
    IndexBuffer(IndexBuffer &&other) noexcept;
    IndexBuffer &operator=(IndexBuffer &&other) noexcept;

    void bindIndexBuffer() const;
    void unbindIndexBuffer() const;
    void bind() const;
    void unbind() const;

    inline unsigned int getCount() const { return count_; }
    inline unsigned int count() const { return count_; }
    inline unsigned int type() const { return type_; }
};
