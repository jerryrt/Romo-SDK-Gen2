#include "com_romotive_library_RomoCommandInterface.h"
#include "PlatformInterface.h"

extern "C" 
{
    JNIEXPORT void JNICALL Java_com_romotive_library_RomoCommandInterface_nativeShutdown(JNIEnv *env, jobject jobj)
    {
        PlatformInterface::getInstance()->shutdown();
    }
    
    JNIEXPORT void JNICALL Java_com_romotive_library_RomoCommandInterface_playMotorCommand(JNIEnv *env, jobject jobj, jint leftValue, jint rightValue)
    {
        PlatformInterface::getInstance()->playMotorCommand((uint8_t) leftValue, (uint8_t) rightValue);
    }
    
    JNIEXPORT void JNICALL Java_com_romotive_library_RomoCommandInterface_playAuxCommand(JNIEnv *env, jobject jobj, jint auxValue)
    {
        PlatformInterface::getInstance()->playAuxCommand((uint8_t) auxValue);    
    }
}