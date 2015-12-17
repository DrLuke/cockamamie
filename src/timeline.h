//
// Created by drluke on 12/15/15.
//

#ifndef COCKAMAMIE_TIMELINE_H
#define COCKAMAMIE_TIMELINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

class timeline {
private:
    double time;
    bool running;
    GLFWwindow* window;

    GLuint vbo;
    GLuint vao;
    GLuint shaderProgram;

    void compileShader();
    void drawQuad(float x1, float y1, float x2, float y2, float r, float g, float b, float a);

public:
    timeline(GLFWwindow* window);
    ~timeline();

    void operator()(){ return work();}
    void work();
    void render();

    double getTime() const
    {
        return time;
    }

    void setTime(double time)
    {
        timeline::time = time;
    }

    bool isRunning() const
    {
        return running;
    }

    void setRunning(bool running)
    {
        timeline::running = running;
    }
};


#endif //COCKAMAMIE_TIMELINE_H
