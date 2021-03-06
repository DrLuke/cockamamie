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


    /*std::vector<keytrack>* newVec = new std::vector<keytrack>();
    std::tuple<shader*, std::vector<keytrack>*, double, double> newScene = std::make_tuple (new shader("/home/drluke/prog/cockamamie/frag.glsl"), newVec, 0.0, 15.0);
    scenes.push_back(newScene);

    std::tie(std::ignore, this->curKeytracks, std::ignore, std::ignore) = this->scenes[0];

    curKeytracks->push_back(keytrack("zoom", 1));
    curKeytracks->push_back(keytrack("offset", 2));
    //curKeytracks->push_back(keytrack("Testname3", 3));
    //curKeytracks->push_back(keytrack("Testname4", 4));
    //curKeytracks->push_back(keytrack("Testname5", 1));

    newVec = new std::vector<keytrack>();
    newScene = std::make_tuple (new shader("/home/drluke/prog/cockamamie/frag2.glsl"), newVec, 15.0, 30.0);
    scenes.push_back(newScene);

    std::tie(std::ignore, this->curKeytracks, std::ignore, std::ignore) = this->scenes[1];

    curKeytracks->push_back(keytrack("Testname1", 1));
    curKeytracks->push_back(keytrack("Testname2", 2));
    curKeytracks->push_back(keytrack("Testname3", 3));
    */
    std::string jsonpath = "/home/drluke/prog/cockamamie/project.json";
    std::ifstream t(jsonpath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string jsonString = buffer.str();
    //std::cout << shaderString << std::endl;
    this->readJSON(&jsonString);
    this->saveJSON(&jsonString);
    std::tie(std::ignore, this->curKeytracks, std::ignore, std::ignore) = this->scenes[0];
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
            switch(kt->getVeclen())
            {
                case 1:
                default:
                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='Val' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal0Callback, &getVal0Callback, kt,
                               stringStream.str().c_str());
                    break;

                case 2:
                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='X' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal0Callback, &getVal0Callback, kt,
                               stringStream.str().c_str());

                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='Y' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal1Callback, &getVal1Callback, kt,
                               stringStream.str().c_str());
                    break;

                case 3:
                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='X' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal0Callback, &getVal0Callback, kt,
                               stringStream.str().c_str());

                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='Y' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal1Callback, &getVal1Callback, kt,
                               stringStream.str().c_str());

                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='Z' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal2Callback, &getVal2Callback, kt,
                               stringStream.str().c_str());
                    break;

                case 4:
                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='X' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal0Callback, &getVal0Callback, kt,
                               stringStream.str().c_str());

                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='Y' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal1Callback, &getVal1Callback, kt,
                               stringStream.str().c_str());

                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='Z' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal2Callback, &getVal2Callback, kt,
                               stringStream.str().c_str());

                    stringStream.str("");
                    stringStream.clear();
                    stringStream << "label='W' group=" << trackname.c_str() << " ";
                    TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_FLOAT, &setVal3Callback, &getVal3Callback, kt,
                               stringStream.str().c_str());
                    break;
            }

            stringStream.str("");
            stringStream.clear();
            stringStream << "label='Keyframe' step=1.0 group=" << trackname.c_str() << " ";

            TwAddVarCB(this->keyframeControls, NULL, TW_TYPE_INT32, &setKeyframeCallback, &GetKeyframeCallback, kt,
                       stringStream.str().c_str());

            stringStream.str("");
            stringStream.clear();
            stringStream << "label='Interpolation' group=" << trackname.c_str() << " ";

            TwType interpType;
            interpType = TwDefineEnumFromString("Interpolation", "Linear,Cosine,Cubic,Cubic Monotonic");

            TwAddVarCB(this->keyframeControls, NULL, interpType, &setInterpCallback, &getInterpCallback, kt,
                       stringStream.str().c_str());
            i++;
        }

        if(!isnan(kt->getRqtime()))
        {
            this->setTime(kt->getRqtime());
            kt->setRqtime(nan(""));
        }
    }
}

void TW_CALL setVal0Callback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setValuePos((float *) value, 0);
}

void TW_CALL getVal0Callback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getValuePos((float *) value, 0);
}
void TW_CALL setVal1Callback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setValuePos((float *) value, 1);
}

void TW_CALL getVal1Callback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getValuePos((float *) value, 1);
}
void TW_CALL setVal2Callback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setValuePos((float *) value, 2);
}

void TW_CALL getVal2Callback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getValuePos((float *) value, 2);
}
void TW_CALL setVal3Callback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setValuePos((float *) value, 3);
}

void TW_CALL getVal3Callback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getValuePos((float *) value, 3);
}

void TW_CALL setKeyframeCallback(const void *value, void *kt)
{
    keytrack* k = (keytrack*)kt;
    k->setStop(true);
    k->setKeyframe((int*)value);
}

void TW_CALL GetKeyframeCallback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    k->getKeyframe((int*)value);
}

void TW_CALL setInterpCallback(const void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    std::string interp;
    switch(*(int*)value)
    {
        case KT_INTERP_LINEAR:
            interp = "linear";
            break;

        case KT_INTERP_COSINE:
            interp = "cosine";
            break;

        case KT_INTERP_CUBIC:
            interp = "cubic";
            break;

        case KT_INTERP_CUBICMONOTONIC:
            interp = "cubicmonotonic";
            break;
    }
    k->setInterp(&interp);
}

void TW_CALL getInterpCallback(void *value, void *kt)
{
    keytrack* k = (keytrack *)kt;
    std::string interp = "";
    k->getInterp(&interp);

    if(interp == "linear")
    {
        *(int*)value = KT_INTERP_LINEAR;
    }
    if(interp == "cosine")
    {
        *(int*)value = KT_INTERP_COSINE;
    }
    if(interp == "cubic")
    {
        *(int*)value = KT_INTERP_CUBIC;
    }
    if(interp == "cubicmonotonic")
    {
        *(int*)value = KT_INTERP_CUBICMONOTONIC;
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
    int width, height;
    glfwGetWindowSize(this->window, &width, &height);


    this->curShader->updateShader();
    this->curShader->render();

    GLint loc = -1;
    for(std::vector<keytrack>::iterator it = this->curKeytracks->begin(); it != this->curKeytracks->end(); ++it)
    {
        loc = glGetUniformLocation(this->curShader->getShaderProgram(), it->getName().c_str());
        if(loc != -1)
        {
            float tempVal[4];
            //it->getValue(tempVal);
            for(int i = 0; i < 4; i++)
            {
                it->getValuePos(&tempVal[i], i);
            }
            switch(it->getVeclen())
            {
                default:
                case 1:
                    glUniform1f(loc, tempVal[0]);
                    break;

                case 2:
                    glUniform2f(loc, tempVal[0], tempVal[1]);
                    break;

                case 3:
                    glUniform3f(loc, tempVal[0], tempVal[1], tempVal[2]);
                    break;

                case 4:
                    glUniform4f(loc, tempVal[0], tempVal[1], tempVal[2], tempVal[3]);
                    break;
            }
        }
    }




    loc = glGetUniformLocation(this->curShader->getShaderProgram(), "res");
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
        //glUniform1f(loc, (GLfloat)4.0);
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

void timeline::readJSON(std::string *jsonstr)
{
    rapidjson::Document jsondoc;
    jsondoc.Parse(jsonstr->c_str());

    rapidjson::Value& jsonscenes = jsondoc["scenes"];   // Get scenes to iterate over

    for(rapidjson::Value::ConstMemberIterator it = jsonscenes.MemberBegin(); it != jsonscenes.MemberEnd(); ++it)
    {
        // Scenes
        std::string sceneName = it->name.GetString();   // Scene Name

        std::vector<keytrack>* newKeytrackVector = new std::vector<keytrack>();
        std::tuple<shader*, std::vector<keytrack>*, double, double> newScene = std::make_tuple (new shader("/home/drluke/prog/cockamamie/frag.glsl"), newKeytrackVector, 0.0, 15.0);
        this->scenes.push_back(newScene);

        for(rapidjson::Value::ConstMemberIterator keytracksContainerIt = it->value.MemberBegin(); keytracksContainerIt != it->value.MemberEnd(); ++keytracksContainerIt)
        {
            // Scene Properties
            std::string memberName = keytracksContainerIt->name.GetString();   // Member Name
            //std::cout << memberName << std::endl;
            if(keytracksContainerIt->value.IsString())
            {
                std::cout << "String: " << keytracksContainerIt->value.GetString() << std::endl;
            }
            else if(keytracksContainerIt->value.IsObject() && memberName == "keytracks")
            {
                for(rapidjson::Value::ConstMemberIterator keytrackIt = keytracksContainerIt->value.MemberBegin(); keytrackIt != keytracksContainerIt->value.MemberEnd(); ++keytrackIt)
                {
                    std::string trackName = keytrackIt->name.GetString();
                    rapidjson::Value::ConstMemberIterator typeIt = keytrackIt->value.FindMember("type");
                    std::string typeString = "UNDEF";
                    if(typeIt != keytrackIt->value.MemberEnd())
                    {
                        typeString = typeIt->value.GetString();
                    }

                    keytrack*newKeytrack;
                    if(typeString == "float")
                    {
                        newKeytrack = new keytrack(trackName, 1);
                    }
                    else if(typeString == "vec2")
                    {
                        newKeytrack = new keytrack(trackName, 2);
                    }
                    else if(typeString == "vec3")
                    {
                        newKeytrack = new keytrack(trackName, 3);
                    }
                    else if(typeString == "vec4")
                    {
                        newKeytrack = new keytrack(trackName, 4);
                    }

                    rapidjson::Value::ConstMemberIterator kfIt = keytrackIt->value.FindMember("keyframes");
                    if(kfIt != keytrackIt->value.MemberEnd())
                    {
                        //std::cout << "Object: " << memberName << std::endl;
                        for(rapidjson::Value::ConstMemberIterator keyframeIt = kfIt->value.MemberBegin(); keyframeIt != kfIt->value.MemberEnd(); ++keyframeIt)
                        {
                            double kfTime = nan("");
                            rapidjson::Value::ConstMemberIterator timeIt = keyframeIt->value.FindMember("t");
                            if(timeIt != keyframeIt->value.MemberEnd())
                            {
                                kfTime = timeIt->value.GetDouble();
                            }
                            std::string kfInterp = "";
                            rapidjson::Value::ConstMemberIterator interpIt = keyframeIt->value.FindMember("interp");
                            if(interpIt != keyframeIt->value.MemberEnd())
                            {
                                kfInterp = interpIt->value.GetString();
                            }

                            // New keyframe
                            float val[4];
                            if(typeString == "float")
                            {
                                rapidjson::Value::ConstMemberIterator valIt = keyframeIt->value.FindMember("0");
                                val[0] = (float) valIt->value.GetDouble();
                            }
                            else if(typeString == "vec2")
                            {
                                rapidjson::Value::ConstMemberIterator valIt = keyframeIt->value.FindMember("0");
                                val[0] = (float) valIt->value.GetDouble();
                                valIt = keyframeIt->value.FindMember("1");
                                val[1] = (float) valIt->value.GetDouble();
                            }
                            else if(typeString == "vec3")
                            {
                                rapidjson::Value::ConstMemberIterator valIt = keyframeIt->value.FindMember("0");
                                val[0] = (float) valIt->value.GetDouble();
                                valIt = keyframeIt->value.FindMember("1");
                                val[1] = (float) valIt->value.GetDouble();
                                valIt = keyframeIt->value.FindMember("2");
                                val[2] = (float) valIt->value.GetDouble();
                            }
                            else if(typeString == "vec4")
                            {
                                rapidjson::Value::ConstMemberIterator valIt = keyframeIt->value.FindMember("0");
                                val[0] = (float) valIt->value.GetDouble();
                                valIt = keyframeIt->value.FindMember("1");
                                val[1] = (float) valIt->value.GetDouble();
                                valIt = keyframeIt->value.FindMember("2");
                                val[2] = (float) valIt->value.GetDouble();
                                valIt = keyframeIt->value.FindMember("3");
                                val[3] = (float) valIt->value.GetDouble();
                            }
                            newKeytrack->addKeyframe(kfTime, val, kfInterp);
                        }
                    }
                    newKeytrackVector->push_back(*newKeytrack);
                }
            }
        }
    }
}

void timeline::saveJSON(std::string *jsonstr)
{
    rapidjson::Document jsondoc;
    jsondoc.SetObject();

    jsondoc.AddMember("projectname", "FIXME", jsondoc.GetAllocator());  // FIXME: Get project name (And maybe also save it :)

    rapidjson::Value scenesObj = rapidjson::Value(rapidjson::kObjectType);
    int sceneCounter = 0;
    for(std::vector<std::tuple<shader*, std::vector<keytrack>*, double, double>>::iterator scenesIt = this->scenes.begin(); scenesIt != this->scenes.end(); ++scenesIt)
    {
        std::string sceneName = std::to_string(sceneCounter);
        rapidjson::Value newScene = rapidjson::Value(rapidjson::kObjectType);

        double sceneStartTime, sceneEndTime;
        shader* sceneShader;

        rapidjson::Value keytrackContainerObj = rapidjson::Value(rapidjson::kObjectType);
        std::vector<keytrack>* curKeytracks;
        std::tie(sceneShader, curKeytracks, sceneStartTime, sceneEndTime) = *scenesIt;

        newScene.AddMember("pathtoshader", rapidjson::Value(sceneShader->getFilepath().c_str(),jsondoc.GetAllocator()).Move(), jsondoc.GetAllocator());   // FIXME: Get from current scene
        newScene.AddMember("start", sceneStartTime , jsondoc.GetAllocator());  // FIXME: Get from current scene
        newScene.AddMember("end", sceneEndTime, jsondoc.GetAllocator());    // FIXME: Get from current scene

        for(std::vector<keytrack>::iterator keytrackIt = curKeytracks->begin(); keytrackIt != curKeytracks->end(); ++keytrackIt)
        {
            rapidjson::Value keytrackObj = rapidjson::Value(rapidjson::kObjectType);
            std::string keytrackName = keytrackIt->getName();

            std::string typeString = "";
            switch(keytrackIt->getVeclen())
            {
                default:
                case 1:
                    keytrackObj.AddMember("type", "float", jsondoc.GetAllocator());
                    break;

                case 2:
                    keytrackObj.AddMember("type", "vec2", jsondoc.GetAllocator());
                    break;

                case 3:
                    keytrackObj.AddMember("type", "vec3", jsondoc.GetAllocator());
                    break;

                case 4:
                    keytrackObj.AddMember("type", "vec4", jsondoc.GetAllocator());
                    break;
            }

            rapidjson::Value keyframeContainerObjs = rapidjson::Value(rapidjson::kObjectType);

            int keyframeCounter = 0;
            for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator keyframeIt = keytrackIt->getKeyframes()->begin(); keyframeIt != keytrackIt->getKeyframes()->end(); ++keyframeIt)
            {
                rapidjson::Value keyframeObj = rapidjson::Value(rapidjson::kObjectType);
                double time;
                float vals[4];
                std::string interp;
                std::tie(time, vals[0], vals[1], vals[2], vals[3], interp) = *keyframeIt;

                keyframeObj.AddMember("t", time, jsondoc.GetAllocator());

                switch(keytrackIt->getVeclen())
                {
                    case 1:
                        keyframeObj.AddMember("0", vals[0], jsondoc.GetAllocator());
                        break;

                    case 2:
                        keyframeObj.AddMember("0", vals[0], jsondoc.GetAllocator());
                        keyframeObj.AddMember("1", vals[1], jsondoc.GetAllocator());
                        break;

                    case 3:
                        keyframeObj.AddMember("0", vals[0], jsondoc.GetAllocator());
                        keyframeObj.AddMember("1", vals[1], jsondoc.GetAllocator());
                        keyframeObj.AddMember("2", vals[2], jsondoc.GetAllocator());
                        break;

                    case 4:
                        keyframeObj.AddMember("0", vals[0], jsondoc.GetAllocator());
                        keyframeObj.AddMember("1", vals[1], jsondoc.GetAllocator());
                        keyframeObj.AddMember("2", vals[2], jsondoc.GetAllocator());
                        keyframeObj.AddMember("3", vals[3], jsondoc.GetAllocator());
                        break;
                }

                std::string keyframeName = std::to_string(keyframeCounter++);
                keyframeContainerObjs.AddMember(rapidjson::Value(keyframeName.c_str(), jsondoc.GetAllocator()).Move(), keyframeObj, jsondoc.GetAllocator());
            }
            keytrackObj.AddMember("keyframes", keyframeContainerObjs, jsondoc.GetAllocator());
            keytrackContainerObj.AddMember(rapidjson::Value(keytrackName.c_str(),jsondoc.GetAllocator()).Move(), keytrackObj, jsondoc.GetAllocator());
        }
        newScene.AddMember("keytracks", keytrackContainerObj, jsondoc.GetAllocator());
        scenesObj.AddMember(rapidjson::Value(sceneName.c_str(),jsondoc.GetAllocator()).Move(), newScene, jsondoc.GetAllocator());
    }
    jsondoc.AddMember("scenes", scenesObj, jsondoc.GetAllocator());

    rapidjson::StringBuffer buf;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
    jsondoc.Accept(writer);

    std::cout << buf.GetString() << std::endl;
}