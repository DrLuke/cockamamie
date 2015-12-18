//
// Created by drluke on 12/17/15.
//

#include "keytrack.h"
#include <iostream>

keytrack::keytrack(std::string name, int veclen) : veclen(veclen), name(name), stop(false), rqtime(nan(""))
{
    for(int i = 0; i < this->veclen; i++)
    {
        this->getsetBuf[i] = 0;
    }
}

keytrack::~keytrack()
{
}

void keytrack::setValue(float* val)
{
    for(int i = 0; i < this->veclen; i++)
    {
        this->getsetBuf[i] = val[i];
    }

    bool onKeyframe = false;
    for(std::list<std::tuple<double, float, float, float, float, std::string>>::iterator it = this->keyframes.begin(); it != this->keyframes.end(); ++it)
    {
        if(this->time == std::get<0>(*it))
        {
            onKeyframe = true;
            //TODO: Set current keyframe to val
        }
    }
    if(!onKeyframe)
    {
        this->addKeyframe(this->time, val);
    }
}

void keytrack::getValue(float* val)
{
    //TODO: Get interpolated value first

    for(int i = 0; i < this->veclen; i++)
    {
        val[i] = this->getsetBuf[i];
    }
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


int keytrack::getMaxKeyframes()
{
    return this->keyframes.size();
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
