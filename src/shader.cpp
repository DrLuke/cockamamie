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
}
