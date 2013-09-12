package com.polysfactory.n3.demo;

import java.io.File;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.WindowManager;

public class MainActivity extends Activity implements CvCameraViewListener2, OnTouchListener {

    // for marker detecting
    private boolean mDrag = false;
    private boolean mSelectFlag = false;
    private boolean mToBeCropped = false;
    private Point mPointSrc, mPointDest;
    private Mat mCropped;
    private static final int LINE_WIDTH = 3;
    private int inWidth;
    private int inHeight;

    enum State {
        DETECTING, TRACKING
    }

    private State state = State.DETECTING;

    static {
        if (!OpenCVLoader.initDebug()) {
            // Handle initialization error
        } else {
            System.loadLibrary("n3_apps");
            System.loadLibrary("n3");
        }
    }

    private static final String TAG = "N3::Activity";

    private Mat mFrame;
    private N3 mNativeDetector;

    private CameraBridgeViewBase mOpenCvCameraView;

    private File mMarkerImage;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.face_detect_surface_view);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.fd_activity_surface_view);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setOnTouchListener(this);

        mMarkerImage = IOUtils.copy(this, R.raw.rby0610, "images", "rby0610.jpg");
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mNativeDetector != null) {
            mNativeDetector.stop();
            mNativeDetector = null;
        }
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        mNativeDetector = new N3(mMarkerImage.getAbsolutePath());
        mNativeDetector.start();
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.enableView();
        }
    }

    public void onDestroy() {
        super.onDestroy();
        mOpenCvCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mFrame = new Mat();
        inWidth = width;
        inHeight = height;
    }

    public void onCameraViewStopped() {
        mFrame.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mFrame = inputFrame.rgba();

        if (state == State.TRACKING) {
            if (mNativeDetector != null) {
                mNativeDetector.process(mFrame);
            }
        } else if (state == State.DETECTING) {
            processDragging(mFrame);
        }

        return mFrame;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "called onCreateOptionsMenu");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "called onOptionsItemSelected; selected item: " + item);
        return true;
    }

    private Rect rectToCrop() {
        int left = normalizeX((int) Math.min(mPointSrc.x, mPointDest.x));
        int right = normalizeX((int) Math.max(mPointSrc.x, mPointDest.x));
        int top = normalizeY((int) Math.min(mPointSrc.y, mPointDest.y));
        int bottom = normalizeY((int) Math.max(mPointSrc.y, mPointDest.y));
        return new Rect(left, top, right - left, bottom - top);
    }

    private int normalizeX(int x) {
        if (x < 0) {
            return 0;
        }
        if (x > inWidth) {
            return inWidth;
        }
        return x;
    }

    private int normalizeY(int y) {
        if (y < 0) {
            return 0;
        }
        if (y > inHeight) {
            return inHeight;
        }
        return y;
    }

    private void processDragging(Mat image) {
        if (mSelectFlag || mDrag) {
            Core.rectangle(image, mPointSrc, mPointDest, new Scalar(255, 0, 0), LINE_WIDTH, 8, 0);
        }
        if (mToBeCropped) {
            Rect roi = rectToCrop();
            Log.d(TAG, "crop image:" + roi);
            mCropped = image.submat(roi);
            mToBeCropped = false;
        }

    }

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        int x = (int) event.getX();
        int y = (int) event.getY();
        switch (event.getAction()) {
        case MotionEvent.ACTION_DOWN: {
            /* left button clicked. ROI selection begins */
            mPointSrc = new Point(x, y);
            mSelectFlag = false;
            break;
        }
        case MotionEvent.ACTION_MOVE: {
            /* mouse dragged. ROI being selected */
            mDrag = true;
            mPointDest = new Point(x, y);
            break;
        }
        case MotionEvent.ACTION_UP: {
            mPointDest = new Point(x, y);
            mDrag = false;
            mSelectFlag = true;
            mToBeCropped = true;
            break;
        }
        default:
            break;
        }
        return true;
    }
}
