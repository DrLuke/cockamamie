//
// Created by drluke on 12/15/15.
//

#include "timeline.h"

timeline::timeline(GLFWwindow* window) : window(window)
{
    this->time = glfwGetTime();
    this->running = true;
    this->currentScene = -9999;

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

    this->keyframeControls = TwNewBar("Keyframe Controls");

    this->trackControls = TwNewBar("Timeline Controls");
    this->timeScale = 1.0;
    this->timelineStart = 0.0;
    this->timelineEnd = 15.0;
    TwAddVarRW(this->trackControls, "Timescale", TW_TYPE_DOUBLE, &this->timeScale,
               " label='Timescale' step=0.01 keyIncr=w keyDecr=s help='Scale of time progression (seconds/second)' ");

    TwAddVarRW(this->trackControls, "time run", TW_TYPE_BOOL8, &this->running,
               " label='Run' keyIncr=SPACE help='Run or Pause timeline (bool)' ");
    TwAddVarRW(this->trackControls, "time fine", TW_TYPE_DOUBLE, &this->time,
               " label='Time fine' step=0.01 keyIncr=w keyDecr=s help='Fine adjustment of time variable (seconds)' group=Time ");
    TwAddVarRW(this->trackControls, "time coarse", TW_TYPE_DOUBLE, &this->time,
               " label='Time coarse' step=0.1 keyIncr=w keyDecr=s help='Coarse adjustment of time variable (seconds)' group=Time ");

    TwAddVarRW(this->trackControls, "timelineStart", TW_TYPE_DOUBLE, &this->timelineStart,
               " label='Timeline Start' step=0.1 keyIncr=w keyDecr=s help='Start time of timeline display (seconds)' group=Timeline");
    TwAddVarRW(this->trackControls, "timelineEnd", TW_TYPE_DOUBLE, &this->timelineEnd,
               " label='Timeline End' step=0.1 keyIncr=w keyDecr=s help='End time of timeline display (seconds)' group=Timeline");
    TwAddVarRO(this->trackControls, "sceneindicator", TW_TYPE_INT32, &this->currentScene,
               " label='Current Scene' help='End time of timeline display (seconds)' group=Timeline");

    std::vector<keytrack>* newVec = new std::vector<keytrack>();
    std::tuple<shader*, std::vector<keytrack>*, double, double> newScene = std::make_tuple (new shader("/home/drluke/prog/cockamamie/frag.glsl"), newVec, 0.0, 10.0);
    scenes.push_back(newScene);

    std::tie(std::ignore, this->curKeytracks, std::ignore, std::ignore) = this->scenes[0];

    curKeytracks->push_back(keytrack("Testname1", 1));
    curKeytracks->push_back(keytrack("Testname2", 2));
    curKeytracks->push_back(keytrack("Testname3", 3));
    curKeytracks->push_back(keytrack("Testname4", 4));
    curKeytracks->push_back(keytrack("Testname5", 1));

    newVec = new std::vector<keytrack>();
    newScene = std::make_tuple (new shader("/home/drluke/prog/cockamamie/frag2.glsl"), newVec, 10.0, 30.0);
    scenes.push_back(newScene);

    std::tie(std::ignore, this->curKeytracks, std::ignore, std::ignore) = this->scenes[1];

    curKeytracks->push_back(keytrack("Testname1", 1));
    curKeytracks->push_back(keytrack("Testname2", 2));
    curKeytracks->push_back(keytrack("Testname3", 3));

}

timeline::~timeline()
{
}

void timeline::work()
{
    this->prevScene = this->currentScene;
    for(std::vector<keytrack>::iterator it = this->curKeytracks->begin(); it != this->curKeytracks->end(); ++it)
    {
        keytrack *kt = &(*it);
        if(kt->isStop())
        {
            this->running = false;
            kt->setStop(false);
        }
    }
    if(this->running)
    {
        double tempTime = glfwGetTime();
        this->time += (tempTime - this->prevTime)*this->timeScale;
        this->prevTime = tempTime;
    }
    else
    {
        this->prevTime = glfwGetTime();
    }

    int i = 0;
    for(std::vector<std::tuple<shader*, std::vector<keytrack>*, double, double>>::iterator it = this->scenes.begin(); it != this->scenes.end(); ++it)
    {
        float start = 0.0;
        float end = 0.0;
        std::tie(std::ignore, std::ignore, start, end) = *it;
        if(this->time > start && this->time <= end)
        {
            this->currentScene = i;
        }

        i++;
    }
    double sceneStart, sceneEnd;

    std::tie(this->curShader, this->curKeytracks, sceneStart, sceneEnd) = this->scenes[this->currentScene];

    if(this->currentScene != prevScene)
    {
        TwRemoveAllVars(this->keyframeControls);
    }

    i = 0;
    for(std::vector<keytrack>::iterator it = this->curKeytracks->begin(); it != this->curKeytracks->end(); ++it)
    {
        keytrack *kt = &(*it);
        kt->setTime(this->time);
        if(this->currentScene != this->prevScene)
        {
            std::string trackname = kt->getName();

            std::ostringstream stringStream;
            stringStream << "label='Val" << trackname << "' step=0.1 group=" << trackname.c_str() << " ";

            stringStream.str("");
            stringStream.clear();
            stringStream << "label='Value' group=" << trackname.c_str() << " ";

            switch(kt->getVeclen())
            {
                case 1:
                default:
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setValCallback, &GetValCallback, kt,
                               stringStream.str().c_str());
                    break;

                case 2:
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setValCallback, &GetValCallback, kt,
                               stringStream.str().c_str());
                    break;

                case 3:
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_COLOR3F, &setValCallback, &GetValCallback, kt,
                               stringStream.str().c_str());
                    break;

                case 4:
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_COLOR4F, &setValCallback, &GetValCallback, kt,
                               stringStream.str().c_str());
                    break;
            }


            //std::cout << keyframecount << std::endl;
            stringStream.str("");
            stringStream.clear();
            //stringStream << "label='Keyframe' min=" << keyframemin << " max=" << keyframemax << " step=0.1 group=" << trackname.c_str() << " ";
            stringStream << "label='Keyframe' step=1 group=" << trackname.c_str() << " ";

            TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_INT32, &setKeyframeCallback, &GetKeyframeCallback, kt,
                       stringStream.str().c_str());
            i++;
        }

        /*
        int keyframecount = kt->getMaxKeyframes();
        int keyframemin = -1;
        int keyframemax = -1;

        if(keyframecount > 0)
        {
            keyframemin = 0;
            keyframemax = keyframecount - 1;
        }*/



        if(!isnan(kt->getRqtime()))
        {
            this->setTime(kt->getRqtime());
            kt->setRqtime(nan(""));
        }
    }
}

void TW_CALL setValCallback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setValue((float*) value);
}

void TW_CALL GetValCallback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getValue((float*)value);
}

void TW_CALL setKeyframeCallback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setKeyframe((int*) value);
}

void TW_CALL GetKeyframeCallback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getKeyframe((int*)value);
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
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);

    this->curShader->updateShader();
    this->curShader->render();


    GLint loc = glGetUniformLocation(this->curShader->getShaderProgram(), "res");
    if(loc != -1)
    {
        glUniform2f(loc, (float)width, (float)height);
    }
    loc = glGetUniformLocation(this->curShader->getShaderProgram(), "t");
    if(loc != -1)
    {
        glUniform1f(loc, (GLfloat)this->getTime());
    }
    loc = glGetUniformLocation(this->curShader->getShaderProgram(), "zoom");
    if(loc != -1)
    {
        glUniform1f(loc, (GLfloat)1.0);
    }



    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glUseProgram (this->shaderProgram);
    glBindVertexArray(this->vao);



    float pixelsizeh = (2.0f/(float)height);
    float pixelsizew = (2.0f/(float)width);


    #define TRACKHEIGHT 20.0f
    float lineheight = pixelsizeh*TRACKHEIGHT;
    int i = 0;
    for(std::vector<keytrack>::iterator it = this->curKeytracks->begin(); it != this->curKeytracks->end(); ++it)
    {
        //Draw alternating lanes
        if(i%2)
        {
            this->drawQuad(-1.0f,-1.0f+(float)i*lineheight, 1.0f,-1.0f+(float)(i+1)*lineheight, 0.3f,0.3f,0.4f,0.6f);
        }
        else
        {
            this->drawQuad(-1.0f,-1.0f+(float)i*lineheight, 1.0f,-1.0f+(float)(i+1)*lineheight, 0.3f,0.3f,0.4f,0.8f);
        }
        // Draw keyframes
        std::vector<double> keyframeTimes;
        it->getKeyframeTimes(&keyframeTimes);

        for(std::vector<double>::iterator tit = keyframeTimes.begin(); tit != keyframeTimes.end(); ++tit)
        {
            float progress = (float)round(((*tit-this->timelineStart)/(this->timelineEnd - this->timelineStart)*2.0) * float(width));
            this->drawQuad(-1.0f + progress/(float)width,-1.0+(float)i*lineheight, -1.0f + pixelsizew + progress/(float)width, -1.0f+(float)(i+1)*lineheight, 0.1f,0.1f,0.8f,0.8f);
        }
        i++;
    }

    // Scene transitions
    float h = (float)curKeytracks->size();
    float sceneStart, sceneEnd;
    std::tie(std::ignore, std::ignore, sceneStart, sceneEnd) = this->scenes[0];
    float progress = (float)round(((sceneStart-timelineStart)/(this->timelineEnd - this->timelineStart)*2.0) * float(width));
    this->drawQuad(-1.0f + progress/(float)width,-1.0, -1.0f + pixelsizew + progress/(float)width, -1.0f + h*lineheight, 0.8f,0.8f,0.1f,1.0f);
    for(std::vector<std::tuple<shader*, std::vector<keytrack>*, double, double>>::iterator it = this->scenes.begin(); it != this->scenes.end(); ++it)
    {
        std::tie(std::ignore, std::ignore, sceneStart, sceneEnd) = *it;
        progress = (float)round(((sceneEnd-timelineStart)/(this->timelineEnd - this->timelineStart)*2.0) * float(width));
        this->drawQuad(-1.0f + progress/(float)width,-1.0, -1.0f + pixelsizew + progress/(float)width, -1.0f + h*lineheight, 0.8f,0.8f,0.1f,0.8f);
    }


    // Time marker
    progress = (float)round(((this->time-this->timelineStart)/(this->timelineEnd - this->timelineStart)*2.0) * float(width));
    this->drawQuad(-1.0f + progress/(float)width,-1.0, -1.0f + pixelsizew + progress/(float)width, -1.0f + h*lineheight, 0.8f,0.1f,0.1f,0.8f);


}

void timeline::drawQuad(float x1, float y1, float x2, float y2, float r, float g, float b, float a)
{
    float vertices[] = {
            x1, y2, r, g, b, a,  // Vertex 1 (X, Y)
            x2, y1, r, g, b, a,  // Vertex 2 (X, Y)
            x2, y2, r, g, b, a,  // Vertex 3 (X, Y)
            x1, y1, r, g, b, a,  // Vertex 4 (X, Y)
            x1, y2, r, g, b, a,  // Vertex 5 (X, Y)
            x2, y1, r, g, b, a   // Vertex 6 (X, Y)
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glDrawArrays (GL_TRIANGLES, 0, 3);
    glDrawArrays (GL_TRIANGLES, 3, 3);

}