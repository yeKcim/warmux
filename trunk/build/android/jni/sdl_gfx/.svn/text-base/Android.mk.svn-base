LOCAL_PATH := $(call my-dir)/../../../lib/sdl_gfx

include $(CLEAR_VARS)

LOCAL_MODULE := sdl_gfx

LOCAL_CFLAGS := -Os

LOCAL_CPP_EXTENSION := .cpp

# Note this simple makefile var substitution, you can find even simpler examples in different Android projects
LOCAL_SRC_FILES := $(notdir $(wildcard $(LOCAL_PATH)/*.c))
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/../sdl/include
LOCAL_SHARED_LIBRARIES := sdl

include $(BUILD_STATIC_LIBRARY)

