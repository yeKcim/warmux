# The libxml2 library bundled with Android is very stripped down.
# For example, it doesn't have the xpath or tree modules we use,
# and therefore we statically link in the object files from our
# own libxml2.

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LIBXML_SOURCE_FILES := \
        SAX.c \
        entities.c \
        encoding.c \
        error.c \
        parserInternals.c \
        parser.c \
        tree.c \
        hash.c \
        list.c \
        xmlIO.c \
        xmlmemory.c \
        uri.c \
        valid.c \
        xlink.c \
        threads.c \
        HTMLparser.c \
        HTMLtree.c \
        xpath.c \
        xpointer.c \
        xinclude.c \
        globals.c \
        xmlstring.c \
        xmlreader.c \
        relaxng.c \
        dict.c \
        SAX2.c \
        legacy.c \
        chvalid.c \
        pattern.c \
        xmlsave.c \
        xmlmodule.c \
        xmlwriter.c

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
LOCAL_SRC_FILES := $(LIBXML_SOURCE_FILES)
LOCAL_MODULE := xml2

include $(BUILD_STATIC_LIBRARY)
