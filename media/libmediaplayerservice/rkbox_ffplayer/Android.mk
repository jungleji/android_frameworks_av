LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_PREBUILT_LIBS := librkboxffplayer.so
LOCAL_MODULE_TAGS := optional
include $(BUILD_MULTI_PREBUILT)
