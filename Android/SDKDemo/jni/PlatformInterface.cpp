/* 
 * File:   NetworkRemote.cpp
 * Author: jamesknight
 * 
 * Created on January 20, 2012, 5:40 PM
 */

#include "PlatformInterface.h"
#include "AudioInterface.h"

PlatformInterface *PlatformInterface::mInstance = NULL;

PlatformInterface *PlatformInterface::getInstance()
{
    if (!mInstance)
        mInstance = new PlatformInterface();
    
    return mInstance;
}

void PlatformInterface::shutdown()
{
    if (mInstance)
    {
        delete mInstance;
        mInstance = NULL;
    }
}

PlatformInterface::PlatformInterface()
{
    mAudioInterface = AudioInterface::create();
}

PlatformInterface::~PlatformInterface() 
{
    if (mAudioInterface)
    {
        delete mAudioInterface;
        mAudioInterface = NULL;
    }
}

void PlatformInterface::playMotorCommand(uint8_t leftValue, uint8_t rightValue)
{
    MotorCommand command = { leftValue, rightValue };
    mAudioInterface->playMotorCommand(command);
}

void PlatformInterface::playAuxCommand(uint8_t auxValue)
{
    AuxCommand command = { auxValue };
    mAudioInterface->playAuxCommand(command);
}



