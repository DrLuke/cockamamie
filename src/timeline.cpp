//
// Created by drluke on 12/15/15.
//

#include "timeline.h"

timeline::timeline(GLFWwindow* window) : window(window)
{
    this->time = glfwGetTime();
    this->running = true;

    this->compileShader();

    float points[] = {
            -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // Vertex 1 (X, Y)
            1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Vertex 2 (X, Y)
            1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Vertex 3 (X, Y)
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // Vertex 4 (X, Y)
            1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,   // Vertex 5 (X, Y)
            1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f    // Vertex 6 (X, Y)
    };

    glUseProgram (this->shaderProgram);

    this->vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    this->vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    GLint posAttrib = glGetAttribLocation(this->shaderProgram, "pos");
    GLint colAttrib = glGetAttribLocation(this->shaderProgram, "color");
    glEnableVertexAttribArray(posAttrib);
    glEnableVertexAttribArray(colAttrib);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (posAttrib, 2, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(0*sizeof(float)));
    glVertexAttribPointer (colAttrib, 4, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(2*sizeof(float)));
}

timeline::~timeline()
{
}

void timeline::work()
{
    if(this->running)
    {
        this->time = glfwGetTime();  // If the timeline is running, get the time from the timer
    }
    else
    {

    }
}

void timeline::compileShader()
{
    const char* vsstring =
            "#version 150\n"
            "in vec2 pos;"
            "in vec4 color;"
            "out vec4 vertColor;"
            "void main() {"
            "  vertColor = color;"
            "  gl_Position = vec4(pos.xy, 0.0, 1.0);"
            "}";

    const char* fsstring =
            "#version 150\n"
            "out vec4 outColor;"
            "in vec4 vertColor;"
            "void main() {"
            "  outColor = vertColor;"
            "}";

    this->shaderProgram = glCreateProgram();

    GLuint vs = glCreateShader (GL_VERTEX_SHADER);
    glShaderSource (vs, 1, &vsstring, NULL);
    glCompileShader (vs);

    GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
    glShaderSource (fs, 1, &fsstring, NULL);
    glCompileShader (fs);

    glAttachShader (this->shaderProgram, fs);
    glAttachShader (this->shaderProgram, vs);
    glLinkProgram (this->shaderProgram);

    int maxLength = 0;
    glGetProgramiv(this->shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
    char infoLog[maxLength];
    glGetProgramInfoLog(this->shaderProgram, maxLength, &maxLength, &infoLog[0]);
    if(maxLength)
    {
        std::cout << "Timeline Shader Infolog reports:" << std::endl;
        std::cout << infoLog << std::endl;
    }
}

void timeline::render()
{
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glUseProgram (this->shaderProgram);
    glBindVertexArray(this->vao);

    int width, height;
    glfwGetWindowSize(this->window, &width, &height);

    // Background box
    this->drawQuad(-1.0f,-1.0f, 1.0f,0.0f, 0.3f,0.3f,0.3f,0.8f);
    this->drawQuad(-0.7f,-0.7f, 0.3f,-0.3f, 0.3f,0.3f,0.3f,0.8f);

}

void timeline::drawQuad(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
    float vertices[] = {
            x1, y2, r, g, b, a,  // Vertex 1 (X, Y)
            x2, y1, r, g, b, a,  // Vertex 3 (X, Y)
            x2, y2, r, g, b, a,  // Vertex 4 (X, Y)
            x1, y1, r, g, b, a,  // Vertex 2 (X, Y)
            x1, y2, r, g, b, a,  // Vertex 4 (X, Y)
            x2, y1, r, g, b, a   // Vertex 2 (X, Y)
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glDrawArrays (GL_TRIANGLES, 0, 3);
    glDrawArrays (GL_TRIANGLES, 3, 3);

}