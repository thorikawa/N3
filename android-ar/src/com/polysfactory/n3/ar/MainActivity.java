package com.polysfactory.n3.ar;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.microedition.khronos.opengles.GL10;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.JavaCameraViewEx;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.Rect;

import android.app.Activity;
import android.content.ContextWrapper;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.Gravity;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.FrameLayout.LayoutParams;

import com.polysfactory.n3.jni.NativeMarkerDetector;
import com.unity3d.player.UnityPlayer;

public class MainActivity extends Activity implements CvCameraViewListener2 {

    private Mat mFrame;

    private JavaCameraViewEx mCameraView;

    private CustomUnityPlayer mUnityPlayer;
    private GLSurfaceView mUnityView;

    private float mScale;
    private int mOffsetX;
    private int mOffsetY;
    private int mHeight;

    private boolean mUnityLoaded = false;

    private NativeMarkerDetector mMarkerDetector;
    // DEBUG
    private static final boolean sUnityIntegrated = true;

    private boolean mFindMarker = false;
    private float[] mTransformation = new float[16];

    private FrameLayout mContainer;

    private static final float[] mCameraParameters = { 357.658935546875F, 357.658935546875F, 319.5F, 179.5F };

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(
                WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON | WindowManager.LayoutParams.FLAG_FULLSCREEN);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.marker_tracking);

        mCameraView = (JavaCameraViewEx) findViewById(R.id.fd_activity_surface_view);
        mCameraView.setCameraIndex(Constants.CAMERA_INDEX);
        mCameraView.setCvCameraViewListener(this);
        mCameraView.setMaxFrameSize(Constants.MAX_FRAME_SIZE_WIDTH, Constants.MAX_FRAME_SIZE_HEIGHT);
        mCameraView.enableFpsMeter();
        mCameraView.disableView();

        if (sUnityIntegrated) {
            mUnityPlayer = new CustomUnityPlayer(this);
            int glesMode = mUnityPlayer.getSettings().getInt("gles_mode", 1);
            mUnityPlayer.init(glesMode, false);

            mUnityView = new GLSurfaceView(getApplication());
            mUnityView.setEGLContextClientVersion(2);
            mUnityView.setZOrderOnTop(true);
            // mUnityView.setZOrderMediaOverlay(true);
            mUnityView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
            mUnityView.setRenderer(mUnityPlayer);
            mUnityView.getHolder().setFormat(PixelFormat.TRANSLUCENT);

            mContainer = (FrameLayout) findViewById(R.id.container);
            LayoutParams lp = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
            mContainer.addView(mUnityView, 0, lp);
        }
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mCameraView != null) {
            mCameraView.disableView();
        }
        if (mUnityPlayer != null) {
            mUnityPlayer.pause();
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        if (!sUnityIntegrated || mUnityLoaded) {
            mMarkerDetector = new NativeMarkerDetector(mCameraParameters[0], mCameraParameters[1],
                    mCameraParameters[2], mCameraParameters[3]);
            mCameraView.enableView();
        }
        if (mUnityPlayer != null) {
            mUnityPlayer.resume();
        }
    }

    public void onUnityLoaded() {
        Log.d(L.TAG, "onUnityLoaded");
        if (!mUnityLoaded) {
            mMarkerDetector = new NativeMarkerDetector(mCameraParameters[0], mCameraParameters[1],
                    mCameraParameters[2], mCameraParameters[3]);
            mCameraView.enableView();
            mUnityLoaded = true;
        }
    }

    public void onDestroy() {
        super.onDestroy();
        mCameraView.disableView();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mFrame = new Mat();
        mScale = mCameraView.getScale();
        Log.d(L.TAG, "scale:" + mCameraView.getScale());
        Log.d(L.TAG, "viewsize:" + mCameraView.getWidth() + "," + mCameraView.getHeight());
        Log.d(L.TAG, "framesize:" + width + "," + height);
        mHeight = mCameraView.getHeight();
        final int screenWidth = (int) (width * mScale);
        final int screenHeight = (int) (height * mScale);
        mOffsetX = (mCameraView.getWidth() - screenWidth) / 2;
        mOffsetY = (mHeight - screenHeight) / 2;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                LayoutParams lp = new LayoutParams(screenWidth, screenHeight, Gravity.CENTER);
                mContainer.updateViewLayout(mUnityView, lp);
            }
        });

        Log.d(L.TAG, "offset:" + mOffsetX + "," + mOffsetY);
    }

    @Override
    public void onCameraViewStopped() {
        // XXX: do we need it??
        mFrame.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mFrame = inputFrame.rgba();
        if (Constants.FLIP) {
            Core.flip(mFrame, mFrame, 1);
        }
        // XXX: temporary fix for mirroring issue
        // Mat flipped = new Mat();
        // Core.flip(mFrame, flipped, 1);

        if (mMarkerDetector != null) {
            List<Mat> transformations = new ArrayList<Mat>();
            // mMarkerDetector.findMarkers(mFrame, transformations);
            Log.d(L.TAG, "mScale=" + mScale);
            mMarkerDetector.findMarkers(mFrame, transformations, 1.0F);
            int count = transformations.size();
            if (count > 0) {
                Mat mat = transformations.get(0);
                synchronized (this) {
                    mat.get(0, 0, mTransformation);
                    // Log.d(L.TAG, "found " + count + " markers");
                    Log.d(L.TAG, Arrays.toString(mTransformation));
                    for (int i = 2; i < 16; i += 4) {
                        mTransformation[i] = -mTransformation[i];
                    }
                    Log.d(L.TAG, "=>" + Arrays.toString(mTransformation));
                }
                mFindMarker = true;
            } else {
                mFindMarker = false;
            }

            if (sUnityIntegrated) {
                // UnityPlayer.UnitySendMessage(UNITY_OBJECT_NAME, UNITY_METHOD_NAME, message);
            }
        }

        return mFrame;
    }

    public float[] getTransformation() {
        synchronized (this) {
            return mTransformation;
        }
    }

    public boolean getFindMarker() {
        return mFindMarker;
    }

    public float getScale() {
        return mScale;
    }

    public float[] getCameraParameters() {
        return mCameraParameters;
    }

    private void normalize(Rect rect) {
        rect.width *= mScale;
        rect.height *= mScale;
        rect.x = (int) (mOffsetX + rect.x * mScale);
        rect.y = (int) (mHeight - (mOffsetY + rect.y * mScale + rect.height));
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        return true;
    }

    class CustomUnityPlayer extends UnityPlayer {
        public CustomUnityPlayer(ContextWrapper context) {
            super(context);
            currentActivity = MainActivity.this;
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            super.onDrawFrame(gl);
        }
    }
}
