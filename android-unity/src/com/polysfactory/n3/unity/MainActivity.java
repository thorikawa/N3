package com.polysfactory.n3.unity;

import java.io.File;

import javax.microedition.khronos.opengles.GL10;

import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Rect;
import org.opencv.highgui.Highgui;

import android.app.Activity;
import android.content.ContextWrapper;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.ImageView.ScaleType;
import android.widget.LinearLayout;

import com.unity3d.player.UnityPlayer;

public class MainActivity extends Activity implements CvCameraViewListener2 {

    private static final String UNITY_OBJECT_NAME = "Miku_Hatsune";

    private static final String UNITY_METHOD_NAME = "JavaMessage";

    private static final int REQUEST_MARKER_CAPTURE = 100;
    private Mat mFrame;
    private N3 mNativeDetector;

    private MyCameraView mCameraView;

    private MenuItem mCaptureRedMenu;
    private MenuItem mCaptureBlueMenu;
    private ViewGroup mMarkerPreview;

    private CustomUnityPlayer mUnityPlayer;
    private GLSurfaceView mUnityView;

    private float mScale;
    private int mOffsetX;
    private int mOffsetY;
    private int mHeight;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(L.TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(
                WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON | WindowManager.LayoutParams.FLAG_FULLSCREEN);
        requestWindowFeature(Window.FEATURE_NO_TITLE);

        setContentView(R.layout.marker_tracking);

        mCameraView = (MyCameraView) findViewById(R.id.fd_activity_surface_view);
        mCameraView.setCameraIndex(Constants.CAMERA_INDEX);
        mCameraView.setCvCameraViewListener(this);
        mCameraView.setMaxFrameSize(Constants.MAX_FRAME_SIZE_WIDTH, Constants.MAX_FRAME_SIZE_HEIGHT);

        mMarkerPreview = (ViewGroup) findViewById(R.id.marker_preview_container);

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

        FrameLayout layout = (FrameLayout) findViewById(R.id.container);
        LayoutParams lp = new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        layout.addView(mUnityView, 0, lp);
    }

    @Override
    public void onPause() {
        super.onPause();
        if (mNativeDetector != null) {
            mNativeDetector.stop();
            mNativeDetector = null;
        }
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
        setupMarkerPreview();
        mNativeDetector = new N3(Marker.RED.getFilePath(this), Marker.BLUE.getFilePath(this));
        mNativeDetector.start();
        if (mCameraView != null) {
            mCameraView.enableView();
        }
        if (mUnityPlayer != null) {
            mUnityPlayer.resume();
        }
    }

    public void onDestroy() {
        super.onDestroy();
        mCameraView.disableView();
    }

    public void onCameraViewStarted(int width, int height) {
        mFrame = new Mat();
        mScale = mCameraView.getScale();
        Log.d(L.TAG, "scale:" + mCameraView.getScale());
        // Log.d(L.TAG, "viewsize:" + mCameraView.getWidth() + "," + mCameraView.getHeight());
        // Log.d(L.TAG, "startsize:" + width + "," + height);
        mHeight = mCameraView.getHeight();
        mOffsetX = (mCameraView.getWidth() - (int) (width * mScale)) / 2;
        mOffsetY = (mHeight - (int) (height * mScale)) / 2;
        Log.d(L.TAG, "offset:" + mOffsetX + "," + mOffsetY);
        mNativeDetector.setSize(width, height, width, height);
    }

    public void onCameraViewStopped() {
        // XXX: do we need it??
        mFrame.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        mFrame = inputFrame.rgba();
        if (Constants.FLIP) {
            Core.flip(mFrame, mFrame, 1);
        }

        if (mNativeDetector != null) {
            // mNativeDetector.process(mFrame);
            MatOfRect rRectMat = new MatOfRect();
            MatOfRect bRectMat = new MatOfRect();
            mNativeDetector.findMarkers(mFrame, rRectMat, bRectMat);
            String message = "";
            message += convertMatOfRect2Message(rRectMat);
            message += "-";
            message += convertMatOfRect2Message(bRectMat);
            UnityPlayer.UnitySendMessage(UNITY_OBJECT_NAME, UNITY_METHOD_NAME, message);
        }

        return mFrame;
    }

    private String convertMatOfRect2Message(MatOfRect rectOfMat) {
        String message = "";
        if (rectOfMat.empty()) {
            message += "X";
        } else {
            Rect rect = rectOfMat.toList().get(0);
            normalize(rect);
            message += rect.x + "_" + rect.y + "_" + rect.width + "_" + rect.height;
        }
        return message;
    }

    private void normalize(Rect rect) {
        rect.width *= mScale;
        rect.height *= mScale;
        rect.x = (int) (mOffsetX + rect.x * mScale);
        rect.y = (int) (mHeight - (mOffsetY + rect.y * mScale + rect.height));
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        mCaptureRedMenu = menu.add(R.string.capture_marker);
        mCaptureBlueMenu = menu.add(R.string.capture_blue);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (mCaptureRedMenu == item) {
            Intent intent = new Intent(this, CaptureActivity.class);
            intent.putExtra(CaptureActivity.EXTRA_KEY_MARKER, Marker.RED);
            startActivityForResult(intent, REQUEST_MARKER_CAPTURE);
        } else if (mCaptureBlueMenu == item) {
            Intent intent = new Intent(this, CaptureActivity.class);
            intent.putExtra(CaptureActivity.EXTRA_KEY_MARKER, Marker.BLUE);
            startActivityForResult(intent, REQUEST_MARKER_CAPTURE);
        }
        return true;
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (REQUEST_MARKER_CAPTURE == requestCode) {
            // TODO reload marker
        }
    }

    private void setupMarkerPreview() {
        if (mMarkerPreview == null) {
            return;
        }
        mMarkerPreview.removeAllViews();
        for (final Marker marker : Marker.values()) {
            File markerFile = marker.getFile(this);
            if (!markerFile.exists()) {
                IOUtils.copy(this, marker.getDefaultRes(), markerFile);
            }

            Mat mat = Highgui.imread(markerFile.getAbsolutePath());
            Bitmap markerBitmap = Bitmap.createBitmap(mat.cols(), mat.rows(), Bitmap.Config.ARGB_8888);
            Utils.matToBitmap(mat, markerBitmap);
            ImageView imageView = new ImageView(this);
            imageView.setMaxWidth(100);
            imageView.setMaxHeight(100);
            int width = 100;
            int height = 100;
            LinearLayout.LayoutParams parms = new LinearLayout.LayoutParams(width, height);
            imageView.setLayoutParams(parms);
            imageView.setScaleType(ScaleType.CENTER_INSIDE);
            imageView.setImageBitmap(markerBitmap);
            imageView.setFocusable(true);
            imageView.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    Intent intent = new Intent(MainActivity.this, CaptureActivity.class);
                    intent.putExtra(CaptureActivity.EXTRA_KEY_MARKER, marker);
                    startActivityForResult(intent, REQUEST_MARKER_CAPTURE);
                }
            });
            mMarkerPreview.addView(imageView);
        }
    }

    class CustomUnityPlayer extends UnityPlayer {
        public CustomUnityPlayer(ContextWrapper context) {
            super(context);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            super.onDrawFrame(gl);
        }
    }
}
