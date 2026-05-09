#include "gl_renderer/renderer.hpp"
#include "gl_get_error/gl_get_error.hpp"

void Renderer::clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::drawArrays(const GLenum mode, const GLsizei count, const GLenum type, const void *indices) const
{
    GLCall(glDrawArrays(mode, 0, count));
}

void Renderer::drawElements(const GLenum mode, const GLsizei count, const GLenum type, const void *indices) const
{
    GLCall(glDrawElements(mode, count, type, indices));
}