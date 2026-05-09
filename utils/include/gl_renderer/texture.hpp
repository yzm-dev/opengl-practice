#pragma once

#include <string>

#include "glad/glad.h"

class Texture
{
private:
    unsigned int renderer_id_;
    std::string file_path_;
    unsigned char *local_buffer_;
    int width_;
    int height_;
    int nr_channels_;

public:
    Texture(const std::string &file_path);
    ~Texture();

    Texture(const Texture &) = delete;
    Texture &operator=(const Texture &) = delete;
    Texture(Texture &&other) noexcept;
    Texture &operator=(Texture &&other) noexcept;

    void bindTexture(unsigned int slot = 0) const;
    void unbindTexture() const;
    void bind(unsigned int slot = 0) const;
    void unbind() const;

    inline int getWidth() const { return width_; }
    inline int getHeight() const { return height_; }
    inline int width() const { return width_; }
    inline int height() const { return height_; }
};
