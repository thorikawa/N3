package com.polysfactory.n3.jni;

import java.util.List;

import org.opencv.core.Mat;

public class NativeMarkerDetector {

    private long mNativeObj;

    public NativeMarkerDetector() {
        mNativeObj = nativeCreateObject();
    }

    public void findMarkers(Mat imageBgra, List<Mat> transformations) {
        Mat transformationsMat = new Mat();
        nativeFindMarkers(mNativeObj, imageBgra.nativeObj, transformationsMat.nativeObj);
        Converter.transformationsMatToList(transformationsMat, transformations);
    }

    public void release() {
        nativeDestroyObject(mNativeObj);
        mNativeObj = 0;
    }

    private native long nativeCreateObject();

    private native void nativeFindMarkers(long thiz, long imageBgra, long transformations);

    private native void nativeDestroyObject(long thiz);
}
