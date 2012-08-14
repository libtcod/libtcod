LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL2
TCOD_PATH := ../libtcod
TCOD_SAMPLE_PATH = $(TCOD_PATH)/samples/

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/$(TCOD_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.cpp \
	jni_glue.c \
	$(TCOD_SAMPLE_PATH)/samples_c.c

LOCAL_SHARED_LIBRARIES := SDL2 tcod

LOCAL_LDLIBS := -lGLESv1_CM -llog

include $(BUILD_SHARED_LIBRARY)
