package com.polysfactory.n3.demo;

import org.opencv.core.Mat;

public class N3 {
    public N3() {
        mNativeObj = nativeCreateObject();
    }

    public void start() {
        nativeStart(mNativeObj);
    }

    public void stop() {
        nativeStop(mNativeObj);
    }

    public void process(Mat imageGray, Mat imageRgba) {
        nativeProcess(mNativeObj, imageGray.getNativeObjAddr(), imageRgba.getNativeObjAddr());
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject();

    private static native void nativeDestroyObject(long thiz);

    private static native void nativeStart(long thiz);

    private static native void nativeStop(long thiz);

    private static native void nativeProcess(long thiz, long imageGray, long imageRgba);
}
