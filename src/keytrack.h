//
// Created by drluke on 12/17/15.
//

#ifndef COCKAMAMIE_KEYTRACK_H
#define COCKAMAMIE_KEYTRACK_H

#include <list>
#include <tuple>
#include <math.h>

class keytrack
{
private:
    std::list<std::tuple<double, float, float, float, float, std::string>> keyframes;

    int veclen;
    std::string name;

public:
    keytrack(int veclen, std::string name);
    ~keytrack();



    std::tuple<double, float, float, float, float, std::string> getNextKeyframeTuple(double time);
    std::tuple<double, float, float, float, float, std::string> getPrevKeyframeTuple(double time);

    void setKeyframe(double time);
    int editKeyframe(double time, float a, float b=nan(""), float c=nan(""), float d=nan(""), std::string="");

    void getInterpvalues(double time);
};


#endif //COCKAMAMIE_KEYTRACK_H
