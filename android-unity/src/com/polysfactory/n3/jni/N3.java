package com.polysfactory.n3.jni;

import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;

public class N3 {
    public N3(String redMarker, String blueMarker) {
        mNativeObj = nativeCreateObject(redMarker, blueMarker);
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

    public void findMarkers(Mat imageRgba, MatOfRect rRect, MatOfRect bRect) {
        nativeFindMarkers(mNativeObj, imageRgba.nativeObj, rRect.nativeObj, bRect.nativeObj);
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    public void setSize(int srcWidth, int srcHeight, int destWidth, int destHeight) {
        nativeSetSize(mNativeObj, srcWidth, srcHeight, destWidth, destHeight);
    }

    private long mNativeObj = 0;

    private static native long nativeCreateObject(String redMarker, String blueMarker);

    private static native void nativeDestroyObject(long thiz);

    private static native void nativeStart(long thiz);

    private static native void nativeStop(long thiz);

    private static native void nativeProcess(long thiz, long imageRgba);

    private static native void nativeFindMarkers(long thiz, long imageRgba, long rRect, long bRect);

    private static native void nativeSetSize(long thiz, int srcWidth, int srcHeight, int destWidth, int destHeight);
}
