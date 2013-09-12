package com.polysfactory.n3.demo;

import org.opencv.core.Mat;

public class N3 {
    public N3(String markerFileName) {
        mNativeObj = nativeCreateObject(markerFileName);
    }

    public void start() {
        nativeStart(mNativeObj);
    }

    public void stop() {
        nativeStop(mNativeObj);
    }

    public void process(Mat imageRgba) {
        nativeProcess(mNativeObj, imageRgba.getNativeObjAddr());
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject(String markerFileName);

    private static native void nativeDestroyObject(long thiz);

    private static native void nativeStart(long thiz);

    private static native void nativeStop(long thiz);

    private static native void nativeProcess(long thiz, long imageRgba);
}
