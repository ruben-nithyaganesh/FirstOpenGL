#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

struct ProgramShaderSource {
    std::string vertex;
    std::string fragment;
};

static ProgramShaderSource ParseProgramShader(const std::string& file) {
    
    enum class ShaderType { NONE = -1, VERTEX = 0, FRAGMENT = 1 };

    std::ifstream stream(file);
    std::string line;
    ShaderType type = ShaderType::NONE;
    std::stringstream ss[2];

    while (getline(stream, line)) {

        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else {
            ss[(int)type] << line << "\n";
        }
    }

    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(const std::string& source, unsigned int type) {
   GLCall(unsigned int id = glCreateShader(type));
   const char* src = source.c_str(); //&source[0];
   GLCall(glShaderSource(id, 1, &src, nullptr));
   GLCall(glCompileShader(id));

   //Error handling
   int result;
   GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
   if (result == GL_FALSE) {
       int length;
       GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
       char* message = (char*)_malloca(length * sizeof(char));
       GLCall(glGetShaderInfoLog(id, length, &length, message));
       std::cout << "Failed to compile "<<(type == GL_VERTEX_SHADER ? "vertex" : "fragment")<<" shader" << std::endl;
       std::cout << message << std::endl;
       GLCall(glDeleteShader(id));
       return 0;
   }


   return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    GLCall(unsigned int program = glCreateProgram());
    unsigned int vertex_shader = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fragment_shader = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    GLCall(glAttachShader(program, vertex_shader));
    GLCall(glAttachShader(program, fragment_shader));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vertex_shader));
    GLCall(glDeleteShader(fragment_shader));

    return program;
}


void initVertexBuffer(const float* positions, unsigned int size) {
    //'buffer' is the ID of the generated buffer
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    //bindBuffer specifies buffer to be drawn
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    GLCall(glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), positions, GL_STATIC_DRAW));

}


int main(void){
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "glewInit() failed" << std::endl;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    

    float positions1[] = {
         -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f, 0.5f,
         -0.5f, 0.5f
    };

    unsigned int ibuff[6] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    float positions2[] = {
        -0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, -0.5f,
    };


    //initVertexBuffer(positions1, sizeof(positions1) / sizeof(float));

    VertexBuffer vb(positions1, 4 * 2 * sizeof(float));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));


    //Index Buffer
    IndexBuffer ib(ibuff, 6);

    ProgramShaderSource shaderSource = ParseProgramShader("resources/shaders/basic.shader");

    unsigned int shader = CreateShader(shaderSource.vertex, shaderSource.fragment);
    GLCall(glUseProgram(shader));
    GLCall(int location = glGetUniformLocation(shader, "u_Color")); 
    ASSERT(location != -1);
    float r = 0.0f;
    float incr = 0.05f;


    
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw call
        
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        if (r > 1.0f) {
            incr = -0.005f;
        }
        else if (r < 0.0f) {
            incr = 0.005f;
        }
        r += incr;
        
        GLCall(glUseProgram(shader));
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        
        GLCall(glBindVertexArray(vao));
        ib.Bind();  

        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}