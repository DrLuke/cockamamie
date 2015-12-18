//
// Created by drluke on 12/17/15.
//

#ifndef COCKAMAMIE_KEYTRACK_H
#define COCKAMAMIE_KEYTRACK_H

#include <list>
#include <tuple>
#include <math.h>
#include <AntTweakBar.h>

class keytrack
{
private:
    std::list<std::tuple<double, float, float, float, float, std::string>> keyframes;

    float getsetBuf[4];
    int veclen;
    std::string name;

    double time;

    bool stop;

public:
    keytrack(std::string name, int veclen);
    ~keytrack();

    std::tuple<double, float, float, float, float, std::string> getNextKeyframeTuple(double time);
    std::tuple<double, float, float, float, float, std::string> getPrevKeyframeTuple(double time);

    void setValue(float* val);
    void TW_CALL getValue(float* val);

    void TW_CALL toggleKeyframe(double time);
    int TW_CALL editKeyframe(double time, float a, float b=nan(""), float c=nan(""), float d=nan(""), std::string="");

    void getInterpvalues(double time);


    int getVeclen() const
    {
        return veclen;
    }

    void setVeclen(int veclen)
    {
        keytrack::veclen = veclen;
    }

    const std::string &getName() const
    {
        return name;
    }

    void setName(const std::string &name)
    {
        keytrack::name = name;
    }

    void setTime(double time)
    {
        keytrack::time = time;
    }

    bool isStop() const
    {
        return stop;
    }

    void setStop(bool stop)
    {
        keytrack::stop = stop;
    }
};


#endif //COCKAMAMIE_KEYTRACK_H
