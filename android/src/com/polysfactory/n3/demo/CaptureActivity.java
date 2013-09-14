package com.polysfactory.n3.demo;

import java.io.File;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnTouchListener;
import android.view.WindowManager;
import android.widget.Button;

public class CaptureActivity extends Activity implements CvCameraViewListener2, OnTouchListener, OnClickListener {

    // for marker detecting
    private boolean mDrag = false;
    private boolean mSelectFlag = false;
    private Point mPointSrc, mPointDest;
    private Mat mCropped;
    private static final int LINE_WIDTH = 3;
    private int inWidth;
    private int inHeight;
    private Paint paint = new Paint();

    Button finishCaptureButton;

    Button captureAgainButton;

    Button finishSelectButton;

    enum State {
        CAMERA, SELECTING, TRACKING
    }

    private State mState = State.CAMERA;

    private Mat mCaptured;

    private Mat mFrame;

    CameraBridgeViewBase mOpenCvCameraView;

    private File mMarkerFile;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.capture_marker);

        finishCaptureButton = (Button) findViewById(R.id.button_capture);
        finishCaptureButton.setOnClickListener(this);
        captureAgainButton = (Button) findViewById(R.id.button_capture_again);
        captureAgainButton.setOnClickListener(this);
        finishSelectButton = (Button) findViewById(R.id.button_select_done);
        finishSelectButton.setOnClickListener(this);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.camera_view);

        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setOnTouchListener(this);

        mMarkerFile = IOUtils.getFilePath(this, Constants.MARKER_FILE_DIR, Constants.MARKER_FILE_NAME);

        startCameraMode();
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mOpenCvCameraView != null) {
            mOpenCvCameraView.disableView();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
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
        mCacheBitmap = Bitmap.createBitmap(inWidth, inHeight, Bitmap.Config.ARGB_8888);
    }

    public void onCameraViewStopped() {
        // XXX: do we need it??
        // mFrame.release();
        // mCacheBitmap.recycle();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mFrame = inputFrame.rgba();
        mCaptured = mFrame.clone();

        Log.d(L.TAG, "onCameraFrame col:" + mFrame.cols() + ", rows:" + mFrame.rows());

        return mFrame;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.d(L.TAG, "called onCreateOptionsMenu");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.d(L.TAG, "called onOptionsItemSelected; selected item: " + item);
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

    @Override
    public boolean onTouch(View v, MotionEvent event) {
        if (mState == State.SELECTING) {
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
                Log.d(L.TAG, "pre clone: cols:" + mCaptured.cols() + ", rows:" + mCaptured.rows());
                Mat image = mCaptured.clone();
                Log.d(L.TAG, "post clone: cols:" + image.cols() + ", rows:" + image.rows());
                mPointDest = new Point(x, y);
                Core.rectangle(image, mPointSrc, mPointDest, new Scalar(255, 0, 0), LINE_WIDTH, 8, 0);
                drawOnCameraView(image);

                mDrag = true;
                break;
            }
            case MotionEvent.ACTION_UP: {
                Mat image = mCaptured.clone();
                mPointDest = new Point(x, y);
                Core.rectangle(image, mPointSrc, mPointDest, new Scalar(255, 0, 0), LINE_WIDTH, 8, 0);
                drawOnCameraView(image);

                mDrag = false;
                mSelectFlag = true;

                Rect roi = rectToCrop();
                Log.d(L.TAG, "crop image:" + roi);
                mCropped = image.submat(roi);

                break;
            }
            default:
                break;
            }
        }
        return true;
    }

    private Bitmap mCacheBitmap;

    private synchronized void drawOnCameraView(Mat mat) {
        Log.d(L.TAG, "col:" + mat.cols() + ", rows:" + mat.rows());
        Log.d(L.TAG, "bmp w:" + mCacheBitmap.getWidth() + ", h:" + mCacheBitmap.getHeight());
        Utils.matToBitmap(mat, mCacheBitmap);
        SurfaceHolder holder = mOpenCvCameraView.getHolder();
        Canvas canvas = holder.lockCanvas();
        canvas.drawBitmap(mCacheBitmap, 0, 0, paint);
        holder.unlockCanvasAndPost(canvas);
    }

    private void startCameraMode() {
        mState = State.CAMERA;
        // mOpenCvCameraView.enableView();
        captureAgainButton.setVisibility(View.GONE);
        finishCaptureButton.setVisibility(View.VISIBLE);
        finishSelectButton.setVisibility(View.GONE);
    }

    private void finishSelecting() {
        Log.d(L.TAG, "imwrite");
        Highgui.imwrite(mMarkerFile.getAbsolutePath(), mCropped);
    }

    private void startSelectMode() {
        mState = State.SELECTING;
        mOpenCvCameraView.disableView();
        captureAgainButton.setVisibility(View.VISIBLE);
        finishCaptureButton.setVisibility(View.GONE);
        finishSelectButton.setVisibility(View.VISIBLE);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
        case R.id.button_capture:
            startSelectMode();
            break;
        case R.id.button_capture_again:
            startCameraMode();
            break;
        case R.id.button_select_done:
            finishSelecting();
            finishActivity(100);
            break;
        default:
            break;
        }
    }

}
