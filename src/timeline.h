//
// Created by drluke on 12/15/15.
//

#ifndef COCKAMAMIE_TIMELINE_H
#define COCKAMAMIE_TIMELINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <list>
#include <tuple>
#include <vector>
#include <AntTweakBar.h>
#include <math.h>

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include "shader.h"
#include "keytrack.h"

void TW_CALL setVal0Callback(const void *value, void *kt);
void TW_CALL getVal0Callback(void *value, void *kt);
void TW_CALL setVal1Callback(const void *value, void *kt);
void TW_CALL getVal1Callback(void *value, void *kt);
void TW_CALL setVal2Callback(const void *value, void *kt);
void TW_CALL getVal2Callback(void *value, void *kt);
void TW_CALL setVal3Callback(const void *value, void *kt);
void TW_CALL getVal3Callback(void *value, void *kt);
void TW_CALL setInterpCallback(const void *value, void *kt);
void TW_CALL getInterpCallback(void *value, void *kt);



void TW_CALL setKeyframeCallback(const void *value, void *kt);
void TW_CALL GetKeyframeCallback(void *value, void *kt);

class timeline {
private:
    double time;
    double prevTime;
    double timeScale;
    double timelineStart;
    double timelineEnd;
    bool running;
    GLFWwindow* window;

    GLuint vbo;
    GLuint vao;
    GLuint shaderProgram;

    std::vector<std::tuple<shader*, std::vector<keytrack>*, double, double>> scenes;
    shader* curShader;
    std::vector<keytrack>* curKeytracks;
    int prevScene;
    int currentScene;


    TwBar *trackControls;
    TwBar *keyframeControls;

    void compileShader();
    void drawQuad(float x1, float y1, float x2, float y2, float r, float g, float b, float a);



public:
    timeline(GLFWwindow* window);
    ~timeline();

    void operator()(){ return work();}
    void work();
    void render();

    void readJSON(std::string* jsonstr);
    void saveJSON(std::string* jsonstr);

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
