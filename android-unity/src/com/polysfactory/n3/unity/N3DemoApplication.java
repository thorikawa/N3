package com.polysfactory.n3.unity;

import org.opencv.android.OpenCVLoader;

import android.app.Application;

public class N3DemoApplication extends Application {
    static {
        if (!OpenCVLoader.initDebug()) {
            // Handle initialization error
        } else {
            System.loadLibrary("n3_apps");
            System.loadLibrary("n3");
        }
    }

    public void onCreate() {
    };
}
