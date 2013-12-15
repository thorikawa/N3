LOCAL_PATH := $(call my-dir)

include ../Apps/Android.mk

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED

include /Users/poly/workspace/OpenCV-2.4.6-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES  := n3_jni.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/../../Apps
LOCAL_LDLIBS     += -llog -ldl
LOCAL_MODULE     := n3
LOCAL_SHARED_LIBRARIES += n3_apps

include $(BUILD_SHARED_LIBRARY)
