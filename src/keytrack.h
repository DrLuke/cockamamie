//
// Created by drluke on 12/17/15.
//

#ifndef COCKAMAMIE_KEYTRACK_H
#define COCKAMAMIE_KEYTRACK_H

#include <list>
#include <tuple>
#include <math.h>
#include <vector>
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
    double rqtime;

    void interpValues();

public:
    keytrack(std::string name, int veclen=1);
    ~keytrack();

    void setKeyframe(int* val);
    void getKeyframe(int* val);

    void setValue(float* val);
    void getValue(float* val);
    void setValuePos(float *val, int pos);
    void getValuePos(float *val, int pos);

    void addKeyframe(double time, float* val, std::string inpstring="");
    void getKeyframeTimes(std::vector<double>* valVec);

    int getVeclen() const
    {
        return veclen;
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

    double getRqtime() const
    {
        return rqtime;
    }

    void setRqtime(double rqtime)
    {
        keytrack::rqtime = rqtime;
    }
};


#endif //COCKAMAMIE_KEYTRACK_H
