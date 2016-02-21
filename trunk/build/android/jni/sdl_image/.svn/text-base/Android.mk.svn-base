LOCAL_PATH := $(call my-dir)/../../../lib/sdl_image

include $(CLEAR_VARS)

LOCAL_MODULE := sdl_image

LOCAL_CFLAGS := -DLOAD_PNG -DLOAD_JPG -Os

LOCAL_CPP_EXTENSION := .cpp

# Note this simple makefile var substitution, you can find even simpler examples in different Android projects
LOCAL_SRC_FILES := $(notdir $(wildcard $(LOCAL_PATH)/*.c))
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/../jpeg \
                    $(LOCAL_PATH)/../png $(LOCAL_PATH)/../sdl/include
LOCAL_STATIC_LIBRARIES := jpeg
LOCAL_SHARED_LIBRARIES := png sdl

LOCAL_LDLIBS := -lz

include $(BUILD_SHARED_LIBRARY)

