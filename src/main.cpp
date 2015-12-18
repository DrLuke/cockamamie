#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <string.h>

#include <AntTweakBar.h>

#include <SOIL.h>


//---
#include "timeline.h"
#include "shader.h"
#include "TwEventGLFW3.h"
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

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // Start AntTweakBar
    TwInit(TW_OPENGL, NULL);

    // Set GLFW event callbacks
    // - Redirect window size changes to the callback function WindowSizeCB
    //glfwSetWindowSizeCallback(WindowSizeCB);
    // - Directly redirect GLFW mouse button events to AntTweakBar
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)TwEventMouseButtonGLFW3);
    // - Directly redirect GLFW mouse position events to AntTweakBar
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)TwEventCursorPosGLFW3);
    // - Directly redirect GLFW mouse wheel events to AntTweakBar
    glfwSetScrollCallback(window, (GLFWscrollfun)TwEventScrollGLFW3);
    // - Directly redirect GLFW key events to AntTweakBar
    glfwSetKeyCallback(window, (GLFWkeyfun)TwEventKeyGLFW3);
    // - Directly redirect GLFW char events to AntTweakBar
    glfwSetCharCallback(window, (GLFWcharfun)TwEventCharModsGLFW3);

    TwBar *myBar;
    myBar = TwNewBar("NameOfMyTweakBar");
    double myVar = 1.0f;
    TwAddVarRW(myBar, "speed", TW_TYPE_DOUBLE, &myVar,
               " label='Rot speed' min=0.2 max=4 step=0.01 keyIncr=w keyDecr=s help='Rotation speed (turns/second)' ");
    float glassColor[4] = {0, 1, 1, 0.2f}; // transparent cyan
// ...
    TwAddVarRW(myBar, "Glass", TW_TYPE_COLOR4F, &glassColor, "");
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit ();

    // Set up opengl debug output
    //glEnable(GL_DEBUG_OUTPUT);
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
    {
        cout << "glDebugMessageCallback not available" << endl;
    }

    // Set up timeline
    timeline* tim = new timeline(window);


    shader* defaultShader = new shader("/home/drluke/prog/cockamamie/frag.glsl");

    int width, height;
    unsigned char* image =
            SOIL_load_image("/home/drluke/prog/cockamamie/tex.jpg", &width, &height, 0, SOIL_LOAD_RGB);

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

        //glfwGetWindowSize(window, &width, &height);

        //defaultShader->render();

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
        loc = glGetUniformLocation(defaultShader->getShaderProgram(), "zoom");
        if(loc != -1)
        {
            glUniform1f(loc, (GLfloat)myVar);
        }

        tim->render();

        TwWindowSize(width, height);
        TwDraw();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    TwTerminate();
    glfwTerminate();
    return 0;
}