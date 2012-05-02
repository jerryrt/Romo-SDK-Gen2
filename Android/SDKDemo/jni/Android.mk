LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# for native audio
LOCAL_LDLIBS    += -lOpenSLES
# for logging
LOCAL_LDLIBS    += -llog

LOCAL_SRC_FILES := \
	AudioInterface.cpp \
	AudioInterfaceOpenSL.cpp \
    com_romotive_library_RomoCommandInterface.cpp \
    PlatformInterface.cpp \
    RomoDebug.cpp \

LOCAL_MODULE := romo

#
# Build it
#
include $(BUILD_SHARED_LIBRARY)
