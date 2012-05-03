//      
//       o      
//       | 
//  //=======\\
//  || o   o ||
//  ||       ||=>
//  ||  ---  ||
//  \\=======//
//
//  ROMOTIVE, INC
//  AudioThread.cpp
//  C++ class to create an audio thread.
//  http://romotive.com
//
//  Created by James Knight.
//  Last Revision Date: 2/28/2012
//  Copyright (c) 2012 Romotive, Inc. All rights reserved.
//

#include "AudioThread.h"

AudioThread::AudioThread()
{
    pthread_mutex_init(&mDirtyMutex, NULL);
    pthread_cond_init(&mDirtyCond, NULL);
    
    mDirty = FALSE;
    
    pthread_mutex_init(&mEffectMutex, NULL);
    pthread_mutex_init(&mMotorMutex, NULL);
    pthread_mutex_init(&mAuxMutex, NULL);
    
    mEffectDirty = FALSE;
    mEffectFilename = std::string();
    
    mEffectId = 0;
    
    mMotorDirty = FALSE;
    mMotorCommand = MotorCommand();
    
    mAuxDirty = FALSE;
    mAuxCommand = AuxCommand();
}

AudioThread::~AudioThread()
{
    pthread_mutex_destroy(&mDirtyMutex);
    pthread_cond_destroy(&mDirtyCond);
    
    pthread_mutex_destroy(&mEffectMutex);
    pthread_mutex_destroy(&mMotorMutex);
    pthread_mutex_destroy(&mAuxMutex);
    
    if (mAudioInterface)
    {
        delete mAudioInterface;
        mAudioInterface = NULL;
    }
}

void AudioThread::run()
{
    mAudioInterface = AudioInterface::create();
    
    
    while (mState == THREAD_RUNNING)
    {
        pthread_mutex_lock(&mDirtyMutex);
        while (!mDirty)
            pthread_cond_wait(&mDirtyCond, &mDirtyMutex);
        pthread_mutex_unlock(&mDirtyMutex);
        
        mAudioInterface->reclaimBuffers();
        
        pthread_mutex_lock(&mEffectMutex);
        if (mEffectDirty)
        {
            if (mEffectId) {
                mEffectId = 0;
            } else {
                mEffectDirty = !(mAudioInterface->playSoundFile(mEffectFilename));
            }
        }
        pthread_mutex_unlock(&mEffectMutex);
        
        pthread_mutex_lock(&mMotorMutex);
        if (mMotorDirty)
        {
            mMotorDirty = !(mAudioInterface->playMotorCommand(mMotorCommand));
        }
        pthread_mutex_unlock(&mMotorMutex);
        
        pthread_mutex_lock(&mAuxMutex);
        if (mAuxDirty)
        {
            mAuxDirty = !(mAudioInterface->playAuxCommand(mAuxCommand));
        }
        pthread_mutex_unlock(&mAuxMutex);
        
        pthread_mutex_lock(&mDirtyMutex);
        mDirty = mEffectDirty || mMotorDirty || mAuxDirty;
        pthread_mutex_unlock(&mDirtyMutex);
    }
}



void AudioThread::playSoundFile(std::string filename)
{
    pthread_mutex_lock(&mEffectMutex);
    
    mEffectFilename = filename;
    mEffectDirty = TRUE;
    
    pthread_mutex_unlock(&mEffectMutex);
    
    pthread_mutex_lock(&mDirtyMutex);
    mDirty = TRUE;
    pthread_mutex_unlock(&mDirtyMutex);
    pthread_cond_signal(&mDirtyCond);
}

void AudioThread::playMotorCommand(uint8_t leftValue, uint8_t rightValue)
{
    pthread_mutex_lock(&mMotorMutex);
    
    mMotorCommand.leftValue = leftValue;
    
    if (rightValue == 0x80)
        mMotorCommand.rightValue = 0x80;
    else
        mMotorCommand.rightValue = 0xFF - rightValue;
    
    mMotorDirty = TRUE;
    
    pthread_mutex_unlock(&mMotorMutex);
    
    pthread_mutex_lock(&mDirtyMutex);
    mDirty = TRUE;   
    pthread_mutex_unlock(&mDirtyMutex);
    pthread_cond_signal(&mDirtyCond);
}

void AudioThread::playAuxCommand(uint8_t auxValue)
{
    pthread_mutex_lock(&mAuxMutex);
    
    mAuxCommand.auxValue = auxValue;
    mAuxDirty = TRUE;
    
    pthread_mutex_unlock(&mAuxMutex);
    pthread_mutex_lock(&mDirtyMutex);
    mDirty = TRUE;
    pthread_mutex_unlock(&mDirtyMutex);
    pthread_cond_signal(&mDirtyCond);
}