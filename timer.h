#ifndef __TIMER_H__
#define __TIMER_H__

using namespace std;
#include <time.h>

class timer
{
public:
    timer()
    {
        reset();
    }
    ~timer(){};

    void reset()
    {
        status=STOP;
        totalTime=0;
        start=0;
    }

    bool run()
    {
        if(status!=STOP && status!=PAUSE)
            return false;
        status=RUN;
        start=clock();
        return true;
    }

    bool pause()
    {
        if(status!=RUN)
            return false;
        status=PAUSE;
        clock_t end=clock();
        totalTime+=(double)(end-start)/CLOCKS_PER_SEC;
        return true;
    }

    double getTime()
    {
        if(status==RUN)
        {
            clock_t tmp=clock();
            totalTime+=(double)(tmp-start)/CLOCKS_PER_SEC;
            start=tmp;
        }
        return totalTime;
    }
private:
    enum{STOP, RUN, PAUSE}status;
    double totalTime;
    clock_t start;
};

#endif
