//
//  AudioInterfaceOpenSL.h
//  Romo-2
//
//  Created by James Knight on 2/28/12.
//  Copyright (c) 2012 Romotive. All rights reserved.
//

#ifndef AUDIOINTERFACE_OPENSL_H
#define AUDIOINTERFACE_OPENSL_H

#ifdef ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "AudioInterface.h"
#include "SyncQueue.h"

void slPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);

class AudioInterfaceOpenSL: public AudioInterface
{
public:    
    static AudioInterfaceOpenSL *mInstance;
    
    static const uint8_t BUFFER_AVAILABLE = 0;
    static const uint8_t BUFFER_UNAVAILABLE = 1;
    
    SyncQueue<uint8_t *> mBufferQueue;
    
    AudioInterfaceOpenSL();
    virtual ~AudioInterfaceOpenSL();
    
    virtual bool playMotorCommand(MotorCommand command);
    virtual bool playAuxCommand(AuxCommand command);
    
    virtual void reclaimBuffers();
    
private:
    uint32_t mBufferIndex;
    bool mBufferStates[BUFFER_COUNT];
    
    SLObjectItf                     mSlEngineObject;
    SLEngineItf                     mSlEngine;
    
    SLObjectItf                     mSlOutputObject;
    
    SLObjectItf                     mSlPlayerObject; 
    SLPlayItf                       mSlPlayer;
    SLAndroidSimpleBufferQueueItf   mSlBufferQueue;
    
    SLVolumeItf                     mSlVolumeInterface;
    
    void createSoundPlayer();
    
    void loadBuffer(uint8_t *commandBuffer, uint32_t bufferSize);
    void startPlay();
};

#endif
#endif