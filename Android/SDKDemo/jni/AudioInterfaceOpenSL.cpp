//
//  AudioInterfaceOpenSL.cpp
//  Romo-2
//
//  Created by James Knight on 2/28/12.
//  Copyright (c) 2012 Romotive. All rights reserved.
//

#include "RomoDebug.h"
#include "AudioInterfaceOpenSL.h"

#ifdef ANDROID
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "SyncQueue.h"

AudioInterfaceOpenSL *AudioInterfaceOpenSL::mInstance = NULL;

void slPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
    free(AudioInterfaceOpenSL::mInstance->mBufferQueue.popElement(FALSE));
}

AudioInterfaceOpenSL::AudioInterfaceOpenSL()
{
    SLuint32 lEngineMixIIDCount = 1;
    
    const SLInterfaceID lEngineMixIIDs[]    = {SL_IID_ENGINE};
    const SLboolean lEngineMixReqs[]        = {SL_BOOLEAN_TRUE};
    const SLuint32 lOutputMixIIDCount       = 0;
    const SLInterfaceID lOutputMixIIDs[]    = {};
    const SLboolean lOutputMixReqs[]        = {};
    
    SLresult lRes;
    lRes = slCreateEngine(&mSlEngineObject, 0, NULL, lEngineMixIIDCount, lEngineMixIIDs, lEngineMixReqs);
    
    lRes=(*mSlEngineObject)->Realize(mSlEngineObject, SL_BOOLEAN_FALSE); 
    lRes=(*mSlEngineObject)->GetInterface(mSlEngineObject, SL_IID_ENGINE, &mSlEngine); 
    
    lRes=(*mSlEngine)->CreateOutputMix(mSlEngine, &mSlOutputObject, lOutputMixIIDCount, lOutputMixIIDs, lOutputMixReqs); 
    lRes=(*mSlOutputObject)->Realize(mSlOutputObject, SL_BOOLEAN_FALSE);
    
    createSoundPlayer();
    
    mInstance = this;
}

AudioInterfaceOpenSL::~AudioInterfaceOpenSL()
{
    if (mSlOutputObject != NULL) 
    { 
        (*mSlOutputObject)->Destroy(mSlOutputObject); 
        mSlOutputObject = NULL; 
    } 
    
    if (mSlEngineObject != NULL) 
    { 
        (*mSlEngineObject)->Destroy(mSlEngineObject);
        mSlEngineObject = NULL; 
        mSlEngine       = NULL; 
    }
    
    if (mSlPlayerObject != NULL)
    {
        (*mSlPlayerObject)->Destroy(mSlPlayerObject);
        mSlPlayerObject = NULL;
        mSlPlayer       = NULL;
        mSlBufferQueue  = NULL;
    }
}

void AudioInterfaceOpenSL::reclaimBuffers()
{
    SLresult result;
    SLuint32 lPlayerState; 
    
    result = (*mSlPlayerObject)->GetState(mSlPlayerObject, &lPlayerState);
    
    if (lPlayerState != SL_PLAYSTATE_PLAYING)
    {
        while (mBufferQueue.getSize() > 0)
            free(mBufferQueue.popElement(FALSE));
    }
}

void AudioInterfaceOpenSL::createSoundPlayer()
{
    SLresult lRes;
    
    SLDataLocator_AndroidSimpleBufferQueue lDataLocatorIn; 
    lDataLocatorIn.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE; 
    
    // At most 3 buffers in the queue:
    lDataLocatorIn.numBuffers = 3; 
    
    SLDataFormat_PCM lDataFormat; 
    lDataFormat.formatType      = SL_DATAFORMAT_PCM; 
    lDataFormat.numChannels     = 2; // Stereo sound. 
    lDataFormat.samplesPerSec   = SL_SAMPLINGRATE_8; 
    lDataFormat.bitsPerSample   = SL_PCMSAMPLEFORMAT_FIXED_8; 
    lDataFormat.containerSize   = SL_PCMSAMPLEFORMAT_FIXED_8;
    lDataFormat.channelMask     = SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT; 
    lDataFormat.endianness      = SL_BYTEORDER_LITTLEENDIAN; 
    
    SLDataSource lDataSource; 
    lDataSource.pLocator = &lDataLocatorIn; 
    lDataSource.pFormat = &lDataFormat; 
    
    SLDataLocator_OutputMix lDataLocatorOut; 
    lDataLocatorOut.locatorType = SL_DATALOCATOR_OUTPUTMIX; 
    lDataLocatorOut.outputMix = mSlOutputObject; 
    
    SLDataSink lDataSink; 
    lDataSink.pLocator = &lDataLocatorOut;
    lDataSink.pFormat = NULL;
    
    SLuint32 lSoundPlayerIIDCount = 3;
    const SLInterfaceID lSoundPlayerIIDs[] = { SL_IID_PLAY, SL_IID_BUFFERQUEUE, SL_IID_VOLUME };
    const SLboolean lSoundPlayerReqs[] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };
    
    lRes = (*mSlEngine)->CreateAudioPlayer(mSlEngine, &mSlPlayerObject, &lDataSource, &lDataSink, lSoundPlayerIIDCount, lSoundPlayerIIDs, lSoundPlayerReqs); 
    
    lRes = (*mSlPlayerObject)->Realize(mSlPlayerObject, SL_BOOLEAN_FALSE);
    lRes = (*mSlPlayerObject)->GetInterface(mSlPlayerObject, SL_IID_PLAY, &mSlPlayer); 
    lRes = (*mSlPlayerObject)->GetInterface(mSlPlayerObject, SL_IID_BUFFERQUEUE, &mSlBufferQueue);
    lRes = (*mSlPlayerObject)->GetInterface(mSlPlayerObject, SL_IID_VOLUME, &mSlVolumeInterface);
    
    SLmillibel maxLevel;
    
    lRes = (*mSlVolumeInterface)->GetMaxVolumeLevel(mSlVolumeInterface, &maxLevel);
    
    lRes = (*mSlVolumeInterface)->SetVolumeLevel(mSlVolumeInterface, maxLevel);
    
    lRes = (*mSlBufferQueue)->RegisterCallback(mSlBufferQueue, slPlayerCallback, NULL);
    
    lRes = (*mSlPlayer)->SetPlayState(mSlPlayer, SL_PLAYSTATE_PLAYING);
}

void AudioInterfaceOpenSL::loadBuffer(uint8_t *commandBuffer, uint32_t bufferSize)
{
    SLresult lRes; 
    
    lRes = (*mSlBufferQueue)->Enqueue(mSlBufferQueue, commandBuffer, bufferSize);
}

bool AudioInterfaceOpenSL::playMotorCommand(MotorCommand command)
{    
    uint8_t *commandBuffer;
    
    if (command.rightValue != 0x80)
        command.rightValue = 0xFF - command.rightValue;
    
    loadMotorCommandBuffer(&commandBuffer, command.leftValue, command.rightValue);
    
    mBufferQueue.pushElement(commandBuffer);
    
    loadBuffer(commandBuffer, mBufferSize * 2);
    startPlay();
    
    return TRUE;
}

bool AudioInterfaceOpenSL::playAuxCommand(AuxCommand command)
{  
    uint8_t *commandBuffer;
    loadAuxCommandBuffer(&commandBuffer, command.auxValue);
    
    mBufferQueue.pushElement(commandBuffer);
    
    loadBuffer(commandBuffer, mBufferSize);
    startPlay();
    
    return TRUE;
}

void AudioInterfaceOpenSL::startPlay()
{
    SLresult lRes;
    
    SLmillibel maxLevel;
    lRes = (*mSlVolumeInterface)->GetMaxVolumeLevel(mSlVolumeInterface, &maxLevel);
    lRes = (*mSlVolumeInterface)->SetVolumeLevel(mSlVolumeInterface, maxLevel);
    
    SLresult result;
    SLuint32 lPlayerState; 
    
    result = (*mSlPlayerObject)->GetState(mSlPlayerObject, &lPlayerState);
    
    if (lPlayerState != SL_PLAYSTATE_PLAYING)
        result = (*mSlPlayer)->SetPlayState(mSlPlayer, SL_PLAYSTATE_PLAYING);
}

#endif



