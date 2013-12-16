package com.polysfactory.n3.jni;

import java.util.List;

import org.opencv.core.Mat;

public class NativeMarkerDetector {

    public void findMarkers(Mat imageBgra, List<Mat> transformations) {

    }

    private native void nativeFindMarkers(long thiz, long imageBgra, long transformations);
}
