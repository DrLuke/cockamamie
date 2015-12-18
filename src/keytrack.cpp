//
// Created by drluke on 12/17/15.
//

#include "keytrack.h"
#include <iostream>

keytrack::keytrack(std::string name, int veclen) : veclen(veclen), name(name), stop(false)
{
    for(int i = 0; i < this->veclen; i++)
    {
        this->getsetBuf[i] = 0;
    }
}

keytrack::~keytrack()
{
}

void TW_CALL keytrack::setValue(float* val)
{
    for(int i = 0; i < this->veclen; i++)
    {
        this->getsetBuf[i] = val[i];
    }
}

void TW_CALL keytrack::getValue(float* val)
{
    for(int i = 0; i < this->veclen; i++)
    {
        val[i] = this->getsetBuf[i];
    }
}
