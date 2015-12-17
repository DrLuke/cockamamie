//
// Created by drluke on 12/15/15.
//

#include "shader.h"

shader::shader(std::string filepath)
{
    this->filepath = filepath;

    this->shaderProgram = glCreateProgram();

    this->shaderstring = this->readFile(filepath);

    this->compileShader();

    float points[] = {
            -1.0f,  1.0f,  0.0f,
            -1.0f, -1.0f,  0.0f,
            1.0f, -1.0f,  0.0f,
            -1.0f,  1.0f,  0.0f,
            1.0f, -1.0f,  0.0f,
            1.0f,  1.0f,  0.0f
    };

    this->vbo = 0;
    glGenBuffers (1, &vbo);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glBufferData (GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW);

    this->vao = 0;
    glGenVertexArrays (1, &vao);
    glBindVertexArray (vao);
    glEnableVertexAttribArray(0);
    glBindBuffer (GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
}

shader::~shader()
{ }

std::string shader::readFile(std::string pathtofile)
{
    std::ifstream t(pathtofile);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string shaderString = buffer.str();
    return shaderString;
}

void shader::updateShader()
{
    std::string buf = readFile(this->filepath);
    if(!(this->shaderstring == buf))
    {
        this->shaderstring = buf;
        this->compileShader();
    }
}

void shader::compileShader()
{
    const char* vsstring =
        "#version 150\n"
        "in vec3 woop;"
        "void main() {"
        "  gl_Position = vec4(woop,1.0);"
        "}";

    const char* fsstring = this->shaderstring.c_str();

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
        std::cout << "Shader Shader Infolog reports:" << std::endl;
        std::cout << infoLog << std::endl;
    }
}

void shader::render()
{
    glBindBuffer (GL_ARRAY_BUFFER, this->vbo);
    glUseProgram (this->shaderProgram);
    glBindVertexArray (this->vao);
    glDrawArrays (GL_TRIANGLES, 0, 3);
    glDrawArrays (GL_TRIANGLES, 3, 5);


}
