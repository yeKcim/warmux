LOCAL_PATH := $(call my-dir)/../../../lib/freetype

include $(CLEAR_VARS)

LOCAL_MODULE := freetype

APP_SUBDIRS := src/autofit src/cache src/tools \
               src/base src/gzip src/sfnt src/truetype src/raster
#$(patsubst $(LOCAL_PATH)/%, %, $(shell find $(LOCAL_PATH)/src -type d))

# Add more subdirs here, like src/subdir1 src/subdir2

LOCAL_CFLAGS := -DFT2_BUILD_LIBRARY

#Change C++ file extension as appropriate
LOCAL_CPP_EXTENSION := .cpp
LOCAL_C_INCLUDES += $(foreach D, $(APP_SUBDIRS), $(LOCAL_PATH)/$(D)) $(LOCAL_PATH)/include
LOCAL_SRC_FILES := $(foreach F, $(APP_SUBDIRS), $(addprefix $(F)/,$(notdir $(wildcard $(LOCAL_PATH)/$(F)/*.c))))
LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)
