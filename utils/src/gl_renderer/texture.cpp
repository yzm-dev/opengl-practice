#include <iostream>
#include <utility>

#include "gl_get_error/gl_get_error.hpp"
#include "gl_renderer/texture.hpp"
#include "stb_image.h"

Texture::Texture(const std::string &file_path)
    : renderer_id_(0), file_path_(file_path), local_buffer_(nullptr),
      width_(0), height_(0), nr_channels_(0)
{
    stbi_set_flip_vertically_on_load(true);
    local_buffer_ = stbi_load(file_path_.c_str(), &width_, &height_, &nr_channels_, 0);
    if (!local_buffer_)
    {
        std::cerr << "Failed to load texture: " << file_path_ << std::endl;
        return;
    }

    GLCall(glGenTextures(1, &renderer_id_));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id_));

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    int format = GL_RGBA;
    if (nr_channels_ == 4)
        format = GL_RGBA;
    else if (nr_channels_ == 3)
        format = GL_RGB;
    else if (nr_channels_ == 1)
        format = GL_RED;
    else
    {
        std::cerr << "Unsupported texture channel count: " << nr_channels_ << std::endl;
        stbi_image_free(local_buffer_);
        local_buffer_ = nullptr;
        return;
    }

    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, local_buffer_));
    GLCall(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(local_buffer_);
    local_buffer_ = nullptr;
}

Texture::~Texture()
{
    if (local_buffer_)
        stbi_image_free(local_buffer_);
    GLCall(glDeleteTextures(1, &renderer_id_));
}

Texture::Texture(Texture &&other) noexcept
    : renderer_id_(other.renderer_id_),
      file_path_(std::move(other.file_path_)),
      local_buffer_(other.local_buffer_),
      width_(other.width_),
      height_(other.height_),
      nr_channels_(other.nr_channels_)
{
    other.renderer_id_ = 0;
    other.local_buffer_ = nullptr;
    other.width_ = 0;
    other.height_ = 0;
    other.nr_channels_ = 0;
}

Texture &Texture::operator=(Texture &&other) noexcept
{
    if (this != &other)
    {
        if (local_buffer_)
            stbi_image_free(local_buffer_);
        GLCall(glDeleteTextures(1, &renderer_id_));

        renderer_id_ = other.renderer_id_;
        file_path_ = std::move(other.file_path_);
        local_buffer_ = other.local_buffer_;
        width_ = other.width_;
        height_ = other.height_;
        nr_channels_ = other.nr_channels_;

        other.renderer_id_ = 0;
        other.local_buffer_ = nullptr;
        other.width_ = 0;
        other.height_ = 0;
        other.nr_channels_ = 0;
    }
    return *this;
}

void Texture::bindTexture(unsigned int slot /*= 0*/) const
{
    GLCall(glActiveTexture(GL_TEXTURE0 + slot));
    GLCall(glBindTexture(GL_TEXTURE_2D, renderer_id_));
}

void Texture::unbindTexture() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::bind(unsigned int slot) const
{
    bindTexture(slot);
}

void Texture::unbind() const
{
    unbindTexture();
}
