//
// Created by drluke on 12/16/15.
//

#ifndef COCKAMAMIE_KEYFRAME_H
#define COCKAMAMIE_KEYFRAME_H

#include <iostream>

class keyframe
{

};

template <typename T>
class keyframeparam
{
private:
    std::string name;
    T param;

public:
    const std::string &getName() const
    {
        return name;
    }

    void setName(const std::string &name)
    {
        keyframeparam::name = name;
    }

    T getParam() const
    {
        return param;
    }

    void setParam(T param)
    {
        keyframeparam::param = param;
    }
};

#endif //COCKAMAMIE_KEYFRAME_H
