/* 
 * File:   PlatformInterface.h
 * Author: jamesknight
 *
 * Created on January 24, 2012, 1:44 PM
 */

#ifndef PLATFORMINTERFACE_H
#define	PLATFORMINTERFACE_H

#include "RomoInclude.h"

class AudioInterface;

class PlatformInterface 
{
protected:
    static PlatformInterface *mInstance;
    
    PlatformInterface();
    
    AudioInterface *mAudioInterface;
    
public:
    static PlatformInterface *getInstance();
    
    void shutdown();
    virtual ~PlatformInterface();
    
    void playMotorCommand(uint8_t leftValue, uint8_t rightValue);
    void playAuxCommand(uint8_t auxValue);
};

#endif	/* PLATFORMINTERFACE_H */

