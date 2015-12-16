//
// Created by drluke on 12/16/15.
//

#ifndef COCKAMAMIE_KEYTRACK_H
#define COCKAMAMIE_KEYTRACK_H

#include <iostream>
#include <list>

template <typename T>
class keytrack_key
{
private:
    T keyvalue;
    double time;

public:
    keytrack_key(T initVal, double time) : keyvalue(initVal), time(time) {};
    ~keytrack_key();

    T getKeyvalue() const
    {
        return keyvalue;
    }

    void setKeyvalue(T keyvalue)
    {
        keytrack_key::keyvalue = keyvalue;
    }

    double getTime() const
    {
        return time;
    }

    void setTime(double time)
    {
        keytrack_key::time = time;
    }
};

template <typename T>
class keytrack
{
private:
    double time;
    std::list<keytrack_key<T>> keys;
    keytrack_key<T> curKey;
    std::string name;

public:
    keytrack();
    ~keytrack();

    void placeKeyframe();

};




#endif //COCKAMAMIE_KEYTRACK_H
