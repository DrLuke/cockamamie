//
// Created by drluke on 12/15/15.
//

#ifndef COCKAMAMIE_TIMELINE_H
#define COCKAMAMIE_TIMELINE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class timeline {
private:
    double time;
    bool running;

public:
    timeline();
    ~timeline();

    void operator()(){ return work();}
    void work();

    double getTime() const
    {
        return time;
    }

    void setTime(double time)
    {
        timeline::time = time;
    }

    bool isRunning() const
    {
        return running;
    }

    void setRunning(bool running)
    {
        timeline::running = running;
    }
};


#endif //COCKAMAMIE_TIMELINE_H
