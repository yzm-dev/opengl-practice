#pragma once

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <csignal>
#include <cstdlib>
#include <iostream>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

// OpenGL Version 4.3及以上 支持 glDebugMessageControl
// void glDebugMessageControl(GLenum source,
//                            GLenum type,
//                            GLenum severity,
//                            GLsizei count,
//                            const GLuint *ids,
//                            GLboolean enabled);

inline void debugBreak()
{
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__clang__)
    __builtin_debugtrap();
#elif defined(__GNUC__)
    __builtin_trap();
#else
    std::abort();
#endif
}

#define ASSERT(x)         \
    do                    \
    {                     \
        if (!(x))         \
            debugBreak(); \
    } while (0)

#define GLCall(x)                                          \
    do                                                     \
    {                                                      \
        glClearError();                                    \
        x;                                                 \
        ASSERT(glGetErrorLogCall(#x, __FILE__, __LINE__)); \
    } while (0)

inline void glClearError()
{
    while (glGetError() != GL_NO_ERROR)
        ;
}

inline bool glGetErrorLogCall(const char *function, const char *file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cerr << "[OpenGL Error] (" << error << "): "
                  << function << ' ' << file << ':' << line << '\n';
        return false;
    }
    return true;
}
