LOCAL_PATH_ORIG := $(LOCAL_PATH)
LOCAL_PATH := /Users/poly/workspace/N3/Apps

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
OPENCV_LIB_TYPE:=SHARED

include /Users/poly/workspace/OpenCV-2.4.6-android-sdk/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES = Tracker.cpp Draw.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH) $(LOCAL_PATH)/.. $(LOCAL_PATH)/../Box2D
LOCAL_MODULE := n3_apps
include $(BUILD_SHARED_LIBRARY)

LOCAL_PATH := $(LOCAL_PATH_ORIG)