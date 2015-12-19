//
// Created by drluke on 12/17/15.
//

#include "keytrack.h"
#include <iostream>

keytrack::keytrack(std::string name, int veclen) : veclen(veclen), name(name), stop(false), rqtime(nan(""))
{
    for(int i = 0; i < this->veclen; i++)
    {
        this->getsetBuf[i] = 0.0f;
    }
}

keytrack::~keytrack()
{
}

void keytrack::setValue(float* val)
{
    if(val!=NULL)
    {
        for(int i = 0; i < this->veclen; i++)
        {
            this->getsetBuf[i] = val[i];
        }
    }

    bool onKeyframe = false;
    for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
    {
        if(this->time == std::get<0>(*it))
        {
            onKeyframe = true;

            std::get<1>(*it) = this->getsetBuf[0];
            std::get<2>(*it) = this->getsetBuf[1];
            std::get<3>(*it) = this->getsetBuf[2];
            std::get<4>(*it) = this->getsetBuf[3];
        }
    }
    if(!onKeyframe)
    {
        this->addKeyframe(this->time, this->getsetBuf);
    }
}

void keytrack::setValuePos(float *val, int pos)
{
    this->getsetBuf[pos] = *val;
    this->setValue(NULL);
}

void keytrack::getValue(float* val)
{
    this->interpValues();

    for(int i = 0; i < this->veclen; i++)
    {
        val[i] = this->getsetBuf[i];
    }
}

void keytrack::getValuePos(float *val, int pos)
{
    this->interpValues();

    *val = this->getsetBuf[pos];
}

void keytrack::setKeyframe(int *val)
{
    int listlen = this->keyframes.size();

    int i = 0;
    for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
    {
        if(i == *val)
        {
            this->rqtime = std::get<0>(*it);
            break;
        }
        i++;
    }
}

void keytrack::getKeyframe(int *val)
{
    int listlen = this->keyframes.size();
    if(listlen == 0)
    {
        *val = -1;
    }
    else
    {
        //std::cout << "Start ----" << std::endl;
        int i = -1;
        for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
        {
            //std::cout << std::get<0>(*it) << std::endl;
            if (this->time > std::get<0>(*it))
            {
                i++;
            }
            else
            {
                //break;
            }
        }
        *val = i;
    }
}

void keytrack::interpValues()
{
    std::tuple<double, float, float, float, float, std::string>* prevFrame = NULL;
    std::tuple<double, float, float, float, float, std::string>* nextFrame = NULL;

    for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
    {
        if(time < std::get<0>(*it) && it == this->keyframes.begin())
        {
            nextFrame = &(*it);
            break;
        }
        if(time < std::get<0>(*it))
        {
            nextFrame = &(*it);
            it--;
            prevFrame = &(*it);
            break;
        }
        std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it2 = it;
        if(++it2 == this->keyframes.end())
        {
            prevFrame = &(*it);
            break;
        }
    }

    double prevTime;
    double nextTime;
    float prevVals[4];
    float nextVals[4];
    std::string interpmethod;

    if(prevFrame == NULL && nextFrame != NULL)  // Before first keyframe
    {
        std::tie(nextTime, nextVals[0], nextVals[1], nextVals[2], nextVals[3], std::ignore) = *nextFrame;
        for(int i = 0; i < this->veclen; i++)
        {
            this->getsetBuf[i] = nextVals[i];
        }
    }
    else if(prevFrame != NULL && nextFrame != NULL) // Neither before first nor after last keyframe
    {
        std::tie(prevTime, prevVals[0], prevVals[1], prevVals[2], prevVals[3], interpmethod) = *prevFrame;
        std::tie(nextTime, nextVals[0], nextVals[1], nextVals[2], nextVals[3], std::ignore) = *nextFrame;

        float alpha = (float)((this->time - prevTime)/(nextTime - prevTime));

        if(true)//interpmethod == "linear")
        {
            for(int i = 0; i < this->veclen; i++)
            {
                this->getsetBuf[i] = (float)(prevVals[i] + (nextVals[i] - prevVals[i])*(1.0f-(1.0f+cos(alpha*3.1415))*0.5));
            }
        }

    }
    else if(prevFrame != NULL && nextFrame == NULL) // After last keyframe
    {
        std::tie(prevTime, prevVals[0], prevVals[1], prevVals[2], prevVals[3], interpmethod) = *prevFrame;
        for(int i = 0; i < this->veclen; i++)
        {
            this->getsetBuf[i] = prevVals[i];
        }
    }

}

void keytrack::addKeyframe(double time, float* val, std::string inpstring)
{
    if(this->keyframes.size() == 0)
    {
        std::tuple<double, float, float, float, float, std::string> newTuple = std::make_tuple(this->time, val[0], val[1], val[2], val[3], inpstring);
        this->keyframes.push_back(newTuple);
    }
    else
    {
        for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
        {
            std::tuple<double, float, float, float, float, std::string> newTuple = std::make_tuple(this->time, val[0], val[1], val[2], val[3], inpstring);
            if(time < std::get<0>(*it))
            {
                this->keyframes.insert(it, newTuple);
                break;
            }
            std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it2 = it;
            if(++it2 == this->keyframes.end())
            {
                this->keyframes.push_back(newTuple);
                break;
            }
        }
    }
}

void keytrack::getKeyframeTimes(std::vector<double>* valVec)
{
    for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
    {
        valVec->push_back(std::get<0>(*it));
    }
}
