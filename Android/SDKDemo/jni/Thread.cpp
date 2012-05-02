/* 
 * File:   Thread.cpp
 * Author: jamesknight
 * 
 * Created on January 20, 2012, 9:59 AM
 */

#include <iostream>
#include "Thread.h"

void *pthreadInit(void *param);

void *pthreadInit(void *param)
{
    Thread *thread = static_cast<Thread *>(param);
    
    thread->run();
    thread->finish();
    
    pthread_exit(NULL);
}

Thread::Thread()
{
    mState = THREAD_NEW;
}

Thread::~Thread() {}

void Thread::start()
{
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    
    mState = THREAD_RUNNING;
    pthread_create(&mHandle, &attr, pthreadInit, this);
    
    pthread_attr_destroy(&attr);
}

void Thread::shutdown()
{
    mState = THREAD_STOPPED;
    pthread_join(mHandle, NULL);
}

void Thread::finish()
{
    mState = THREAD_FINISHED;
}

