/* 
 * File:   RomotiveDebug.cpp
 * Author: jimmyknauer
 *
 * Created on November 8, 2011, 12:34 PM
 */

#include <iostream>
#include "RomoDebug.h"
#include "RomoInclude.h"
#ifdef ANDROID
#include <android/log.h>
#endif

#define ROMO_DEBUG TRUE


void RomoDebug::printDebug(const char *tag, const char *message)
{
    if (ROMO_DEBUG)
    {
#ifdef __ANDROID__
        __android_log_write(ANDROID_LOG_DEBUG, tag, message);
        
#else   // Console:
        printf("%s: %s\n", tag, message);
        
#endif
    }
        
}

