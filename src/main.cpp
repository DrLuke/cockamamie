#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string.h>


//---
#include "timeline.h"
#include "shader.h"
//---

using namespace std;

void APIENTRY openglCallbackFunction(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar* message,
                                     const void* userParam)
{

    cout << "---------------------opengl-callback-start------------" << endl;
    cout << "message: "<< message << endl;
    cout << "type: ";
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:
            cout << "ERROR";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            cout << "DEPRECATED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            cout << "UNDEFINED_BEHAVIOR";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            cout << "PORTABILITY";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            cout << "PERFORMANCE";
            break;
        case GL_DEBUG_TYPE_OTHER:
            cout << "OTHER";
            break;
        default:
            break;
    }
    cout << endl;

    cout << "id: " << id << endl;
    cout << "severity: ";
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_LOW:
            cout << "LOW";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            cout << "MEDIUM";
            break;
        case GL_DEBUG_SEVERITY_HIGH:
            cout << "HIGH";
            break;
        default:
            break;
    }
    cout << endl;
    cout << "---------------------opengl-callback-end--------------" << endl;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    window = glfwCreateWindow(640, 480, "Cockamamie", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit ();

    // Set up opengl debug output
    glEnable(GL_DEBUG_OUTPUT);
    if(glDebugMessageCallback){
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE,
                              GL_DONT_CARE,
                              GL_DONT_CARE,
                              0,
                              &unusedIds,
                              true);
    }
    else
        cout << "glDebugMessageCallback not available" << endl;

    // Set up timeline
    timeline* tim = new timeline();

    // Set up Vertices for quad
    float points[] = {
            -1.0f,  1.0f,  0.0f,
            -1.0f, -1.0f,  0.0f,
             1.0f, -1.0f,  0.0f,
            -1.0f,  1.0f,  0.0f,
             1.0f, -1.0f,  0.0f,
             1.0f,  1.0f,  0.0f
    };

    GLuint vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, 18 * sizeof (float), points, GL_STATIC_DRAW);

    GLuint vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray (0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    const char* vertex_shader =
            "#version 150\n"
            "in vec3 woop;"
            "void main() {"
            "  gl_Position = vec4(woop,1.0);"
            "}";

    shader* defaultShader = new shader("/home/drluke/prog/cockamamie/frag.glsl");

    /*GLint maxLength = 30000;

    glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &maxLength);
    char infoLog[maxLength];
    glGetProgramInfoLog(shader_program, maxLength, &maxLength, &infoLog[0]);

    std::cout << infoLog << std::endl;*/

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        tim->work(); // Tick timeline
        defaultShader->updateShader();

        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        //glRotatef((float) glfwGetTime() * 50.f, 0.f, 0.f, 1.f);

        glfwGetWindowSize(window, &width, &height);

        GLint loc = glGetUniformLocation(defaultShader->getShaderProgram(), "res");
        if(loc != -1)
        {
            glUniform2f(loc, (float)width, (float)height);
        }
        loc = glGetUniformLocation(defaultShader->getShaderProgram(), "t");
        if(loc != -1)
        {
            glUniform1f(loc, (GLfloat)tim->getTime());
        }

        glUseProgram (defaultShader->getShaderProgram());
        glBindVertexArray (vao);
        // draw points 0-3 from the currently bound VAO with current in-use shader
        glDrawArrays (GL_TRIANGLES, 0, 3);
        glDrawArrays (GL_TRIANGLES, 3, 5);

        /*glGetProgramiv(shader_program, GL_INFO_LOG_LENGTH, &maxLength);
        glGetProgramInfoLog(shader_program, maxLength, &maxLength, &infoLog[0]);
        if(maxLength)
        {
            std::cout << infoLog << std::endl;
        }*/

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}