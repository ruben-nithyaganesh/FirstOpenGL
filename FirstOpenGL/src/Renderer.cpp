
#include "Renderer.h"
#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR);
}

bool GLCheckError(const char* function, const char* file, int line) {
    while (GLenum error = glGetError()) {
        std::cout << "OpenGL error: " << std::dec << error << " " << std::endl;
        std::cout << "Function:     " << function << std::endl;
        std::cout << "File:         " << file << " on line " << std::dec << line << std::endl;
        return false;
    }
    return true;
}

