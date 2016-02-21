OTHER_PATH := $(call my-dir)
LIBS_PATH  := $(OTHER_PATH)/../../../lib
LOCAL_PATH := $(OTHER_PATH)/../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := application

SRC_SRCS := *.cpp ai/*.cpp character/*.cpp game/*.cpp graphic/*.cpp gui/*.cpp \
            gui/big/*.cpp include/*.cpp interface/*.cpp map/*.cpp menu/*.cpp \
            network/*.cpp object/*.cpp particles/*.cpp replay/*.cpp \
            sound/*.cpp team/*.cpp tool/*.cpp weapon/*.cpp

WMX_SRCS := action/*.cpp base/*.cpp maths/*.cpp net/*.cpp tools/*.cpp

FP_SRCS  := *.cpp

APP_SRCS := $(addprefix src/, $(SRC_SRCS)) \
            $(addprefix lib/warmux/, $(WMX_SRCS)) \
            $(addprefix lib/fixedpoint/, $(FP_SRCS)) \

LOCAL_CFLAGS := -fno-exceptions -fno-rtti \
                -DINSTALL_DATADIR=\"./data/\" \
                -DINSTALL_LOCALEDIR=\"./locale\" \
                -DFONT_FILE=\"data/font/Ubuntu-R.ttf\" \
                -DSDL_JAVA_PACKAGE_PATH=$(SDL_JAVA_PACKAGE_PATH)


#Change C++ file extension as appropriate
LOCAL_CPP_EXTENSION := .cpp
LOCAL_SRC_FILES := $(foreach F, $(APP_SRCS), \
                     $(addprefix $(dir $(F)), $(notdir $(wildcard $(LOCAL_PATH)/$(F)))))
LOCAL_C_INCLUDES += $(LIBS_PATH)/sdl/include $(LIBS_PATH)/sdl_mixer \
                    $(LIBS_PATH)/sdl_image $(LIBS_PATH)/sdl_gfx \
                    $(LIBS_PATH)/sdl_ttf $(LOCAL_PATH)/src \
                    $(LOCAL_PATH)/lib/fixedpoint $(LIBS_PATH)/png \
                    $(LIBS_PATH)/sdl_net $(LIBS_PATH)/xml2/include \
                    $(LIBS_PATH)/intl $(LOCAL_PATH)/lib/warmux/include

LOCAL_STATIC_LIBRARIES := sdl_net sdl_gfx sdl_ttf xml2 freetype intl
LOCAL_SHARED_LIBRARIES := sdl sdl_mixer sdl_image png

LOCAL_LDLIBS := -lGLESv1_CM -ldl -llog -lz -lGLESv1_CM


LIBS_WITH_LONG_SYMBOLS := $(strip $(shell \
 for f in $(LOCAL_PATH)/../../libs/armeabi/*.so ; do \
   if echo $$f | grep "libapplication[.]so" > /dev/null ; then \
     continue ; \
   fi ; \
   if [ -e "$$f" ] ; then \
     if nm -g $$f | cut -c 12- | egrep '.{128}' > /dev/null ; then \
       echo $$f | grep -o 'lib[^/]*[.]so' ; \
     fi ; \
   fi ; \
 done \
) )

ifneq "$(LIBS_WITH_LONG_SYMBOLS)" ""
$(foreach F, $(LIBS_WITH_LONG_SYMBOLS), \
$(info Library $(F): abusing symbol names are: \
$(shell nm -g $(LOCAL_PATH)/../../libs/armeabi/$(F) | cut -c 12- | egrep '.{128}' ) ) \
$(info Library $(F) contains symbol names longer than 128 bytes, \
YOUR CODE WILL DEADLOCK WITHOUT ANY WARNING when you'll access such function - \
please make this library static to avoid problems. ) )
$(error Detected libraries with too long symbol names. Remove all files under project/libs/armeabi, make these libs static, and recompile)
endif

include $(BUILD_SHARED_LIBRARY)
