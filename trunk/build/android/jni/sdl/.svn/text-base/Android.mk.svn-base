LOCAL_PATH := $(call my-dir)/../../../lib/sdl

include $(CLEAR_VARS)

LOCAL_MODULE := $(lastword $(subst /, ,$(LOCAL_PATH)))

ifndef SDL_JAVA_PACKAGE_PATH
$(error Please define SDL_JAVA_PACKAGE_PATH to the path of your Java package with dots replaced with underscores, for example "com_example_SanAngeles")
endif

LOCAL_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_CFLAGS := -Os \
	-DSDL_JAVA_PACKAGE_PATH=$(SDL_JAVA_PACKAGE_PATH) \
	-DSDL_CURDIR_PATH=\"$(SDL_CURDIR_PATH)\" \
	-DSDL_TRACKBALL_KEYUP_DELAY=$(SDL_TRACKBALL_KEYUP_DELAY) \
	-DSDL_VIDEO_RENDER_RESIZE_KEEP_ASPECT=$(SDL_VIDEO_RENDER_RESIZE_KEEP_ASPECT) \
	-DSDL_VIDEO_RENDER_RESIZE=$(SDL_VIDEO_RENDER_RESIZE) \
	-DSDL_ANDROID_KEYCODE_0=RETURN -DSDL_ANDROID_KEYCODE_1=RETURN \
	-DSDL_ANDROID_KEYCODE_2=LSHIFT -DSDL_ANDROID_KEYCODE_3=LCTRL \
	-DSDL_ANDROID_KEYCODE_4=F9 -DSDL_ANDROID_KEYCODE_5=ESCAPE \
	-DSDL_ANDROID_KEYCODE_6=SPACE -DSDL_ANDROID_KEYCODE_7=M \
	-DSDL_ANDROID_KEYCODE_8=DELETE -DSDL_ANDROID_KEYCODE_9=BACKSPACE \
	-DSDL_ANDROID_KEYCODE_10=S


SDL_SRCS := \
	src/*.c \
	src/audio/*.c \
	src/cpuinfo/*.c \
	src/events/*.c \
	src/file/*.c \
	src/joystick/*.c \
	src/stdlib/*.c \
	src/thread/*.c \
	src/timer/*.c \
	src/video/*.c \
	src/main/*.c \
	src/thread/pthread/*.c \
	src/timer/unix/*.c \
	src/audio/android/*.c \
	src/video/android/*.c \
	src/video/android/*.cpp \
	src/loadso/dlopen/*.c \
	src/video/memset.arm.S

# TODO: use libcutils for atomic operations, but it's not included in NDK

#	src/atomic/linux/*.c \
#	src/power/linux/*.c \
#	src/joystick/android/*.c \
#	src/haptic/android/*.c \
#	src/libm/*.c \

LOCAL_CPP_EXTENSION := .cpp

# Note this "simple" makefile var substitution, you can find even more complex examples in different Android projects
LOCAL_SRC_FILES := $(foreach F, $(SDL_SRCS), $(addprefix $(dir $(F)),$(notdir $(wildcard $(LOCAL_PATH)/$(F)))))

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog

include $(BUILD_SHARED_LIBRARY)
