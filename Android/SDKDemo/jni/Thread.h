/* 
 * File:   Thread.h
 * Author: jamesknight
 *
 * Created on January 20, 2012, 9:59 AM
 */

#ifndef THREAD_H
#define	THREAD_H

#include "RomoInclude.h"

class Thread 
{
public:
    enum ThreadState
    {
        THREAD_NEW      = 0,
        THREAD_RUNNING  = 1,
        THREAD_PAUSED   = 2,
        THREAD_STOPPED  = 3,
        THREAD_FINISHED = 4
    };
    
protected:    
    ThreadState mState;
    pthread_t mHandle;
    
    Thread();
    virtual ~Thread();
    
public:    
    void start();
    void shutdown();
    void finish();
    
    virtual void run() = 0;
    
    bool getStatus();
};

#endif	/* THREAD_H */

