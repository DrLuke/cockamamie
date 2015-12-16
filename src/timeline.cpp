//
// Created by drluke on 12/15/15.
//

#include "timeline.h"

timeline::timeline()
{
    this->time = glfwGetTime();
    this->running = true;
}

timeline::~timeline()
{
}

void timeline::work()
{
    if(this->running)
    {
        this->time = glfwGetTime();  // If the timeline is running, get the time from the timer
    }
    else
    {

    }
}

