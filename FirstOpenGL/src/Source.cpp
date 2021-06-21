#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

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
   unsigned int id = glCreateShader(type);
   const char* src = source.c_str(); //&source[0];
   glShaderSource(id, 1, &src, nullptr);
   glCompileShader(id);

   //Error handling
   int result;
   glGetShaderiv(id, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE) {
       int length;
       glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
       char* message = (char*)_malloca(length * sizeof(char));
       glGetShaderInfoLog(id, length, &length, message);
       std::cout << "Failed to compile "<<(type == GL_VERTEX_SHADER ? "vertex" : "fragment")<<" shader" << std::endl;
       std::cout << message << std::endl;
       glDeleteShader(id);
       return 0;
   }


   return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vertex_shader = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fragment_shader = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}


void initVertexBuffer(const float* positions, unsigned int size) {
    //'buffer' is the ID of the generated buffer
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    //bindBuffer specifies buffer to be drawn
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
}


int main(void){
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

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

    float positions2[] = {
        -0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, -0.5f,
    };


    initVertexBuffer(positions1, sizeof(positions1) / sizeof(float));


    //Index Buffer
    //'buffer' is the ID of the generated buffer
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    //bindBuffer specifies buffer to be drawn
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), ibuff, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);


    ProgramShaderSource shaderSource = ParseProgramShader("resources/shaders/basic.shader");

    unsigned int shader = CreateShader(shaderSource.vertex, shaderSource.fragment);
    glUseProgram(shader);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Draw call
        
        //glDrawArrays(GL_TRIANGLES, 0, 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                      
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}