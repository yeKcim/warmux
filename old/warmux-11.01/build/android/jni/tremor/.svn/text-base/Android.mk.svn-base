LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES = \
 tremor/bitwise.c \
 tremor/codebook.c \
 tremor/dsp.c \
 tremor/floor0.c \
 tremor/floor1.c \
 tremor/floor_lookup.c \
 tremor/framing.c \
 tremor/info.c \
 tremor/mapping0.c \
 tremor/mdct.c \
 tremor/misc.c \
 tremor/res012.c \
 tremor/vorbisfile.c

LOCAL_CFLAGS+= -Os

ifeq ($(TARGET_ARCH),arm)
LOCAL_CFLAGS+= -D_ARM_ASSEM_
endif

LOCAL_C_INCLUDES:= \
 $(LOCAL_PATH)/tremor

LOCAL_ARM_MODE := arm

LOCAL_MODULE := tremor

include $(BUILD_STATIC_LIBRARY)
