//
// Created by drluke on 12/15/15.
//

#ifndef COCKAMAMIE_SHADER_H
#define COCKAMAMIE_SHADER_H

#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/glew.h>

class shader
{
private:
    std::string filepath;
    std::string shaderstring;

    GLuint shaderProgram;

    std::string readFile(std::string pathtofile);
    void compileShader();


public:
    shader(std::string filepath);
    ~shader();

    void updateShader();

    GLuint getShaderProgram() const
    {
        return shaderProgram;
    }

};


#endif //COCKAMAMIE_SHADER_H
