/* 
 * File:   SyncQueue.h
 * Author: jamesknight
 *
 * Created on January 20, 2012, 3:31 PM
 */

#ifndef SYNCQUEUE_H
#define	SYNCQUEUE_H

#include <deque>
#include "RomoDebug.h"

/*
 * SyncQueue:
 * A template-class used to provide a thread-safe Queue.
 */
template <class T> 
class SyncQueue
{
protected:
    static const char *TAG() {return "SyncQueue";}
    
    pthread_cond_t mCond;
    pthread_mutex_t mMutex;
    std::deque<T> *mDeque;
   
public:
    SyncQueue();
    ~SyncQueue();
    
    inline int getSize()
    {
        return mDeque->size();
    }
    
    T popElement(bool shouldBlock);
    void pushElement(T element);
    
    void deleteElements();
};

/*
 * SyncQueue<T>():
 * Instantiates a SyncQueue to hold items of Class 'T'. 
 */
template <class T>
SyncQueue<T>::SyncQueue() 
{
    pthread_cond_init(&mCond, NULL);
    pthread_mutex_init(&mMutex, NULL);
    mDeque = new std::deque<T>();
}

/*
 * ~SyncQueue<T>():
 * Empties the 
 */
template <class T>
SyncQueue<T>::~SyncQueue() 
{
    pthread_cond_destroy(&mCond);
    pthread_mutex_destroy(&mMutex);
    if (mDeque)
    {
        delete mDeque;
        mDeque = NULL;
    }
}

template <class T>
T SyncQueue<T>::popElement(bool shouldBlock)
{
    T object = NULL;
    
    if (shouldBlock)
    {        
        pthread_mutex_lock(&mMutex);
        
        while (mDeque->size() == 0)
            pthread_cond_wait(&mCond, &mMutex);
        object = mDeque->front();
        mDeque->pop_front();
        
        pthread_mutex_unlock(&mMutex);        
    }
    else
    {
        pthread_mutex_lock(&mMutex);
        
        if (mDeque->size() > 0)
        {
            object = mDeque->front();
            mDeque->pop_front();
        }
        
        pthread_mutex_unlock(&mMutex);        
    }
    
    return object;
}

template <class T>
void SyncQueue<T>::pushElement(T object)
{
    pthread_mutex_lock(&mMutex);
    mDeque->push_back(object);
    pthread_mutex_unlock(&mMutex);
    
    pthread_cond_signal(&mCond);
}

template <class T>
void SyncQueue<T>::deleteElements()
{
    while (!mDeque->empty())
    {
        T front = mDeque->front();
        mDeque->pop_front();
        delete front;
    }
}
#endif	/* SYNCQUEUE_H */

