#pragma once

#include <vector>

struct VertexBufferElement
{
    unsigned int type;
    unsigned int count;
    unsigned char normalized;
    static unsigned int getStrideOfType(unsigned int type);
};

class VertexBufferLayout
{
private:
    std::vector<VertexBufferElement> elements_;
    unsigned int stride_;

public:
    VertexBufferLayout();
    ~VertexBufferLayout();

    template <typename T>
    void push(unsigned int count)
    {
        static_assert(sizeof(T) == 0, "Unsupported type for VertexArrayLayout::push");
    }

    const std::vector<VertexBufferElement> &getElements() const;
    unsigned int getStride() const;
};

template <>
void VertexBufferLayout::push<float>(unsigned int count);

template <>
void VertexBufferLayout::push<unsigned int>(unsigned int count);

template <>
void VertexBufferLayout::push<unsigned char>(unsigned int count);
