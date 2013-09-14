package com.polysfactory.n3.demo;

import java.io.File;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewFrame;
import org.opencv.android.CameraBridgeViewBase.CvCameraViewListener2;
import org.opencv.core.Mat;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.WindowManager;
import android.widget.ImageView;

public class MainActivity extends Activity implements CvCameraViewListener2 {

    private static final int REQUEST_MARKER_CAPTURE = 100;
    private Mat mFrame;
    private N3 mNativeDetector;

    private CameraBridgeViewBase mOpenCvCameraView;

    private MenuItem mCaptureMenu;
    private ImageView mMarkerPreview;
    private MenuItem mCaptureBlueMenu;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(L.TAG, "called onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        setContentView(R.layout.marker_tracking);

        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.fd_activity_surface_view);
        mOpenCvCameraView.setCameraIndex(CameraBridgeViewBase.CAMERA_ID_FRONT);
        mOpenCvCameraView.setCvCameraViewListener(this);

        mMarkerPreview = (ImageView) findViewById(R.id.marker_preview);

        for (Marker marker : Marker.values()) {
            File markerFile = marker.getFile(this);
            if (!markerFile.exists()) {
                IOUtils.copy(this, Constants.DEFAULT_MARKER_IMAGE_RES, markerFile);
            }
        }
        Bitmap marker = BitmapFactory.decodeFile(Marker.RED.getFilePath(this));
        mMarkerPreview.setImageBitmap(marker);
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
        mNativeDetector = new N3(Marker.RED.getFilePath(this), Marker.BLUE.getFilePath(this));
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
        mNativeDetector.setSize(width, height, width, height);
    }

    public void onCameraViewStopped() {
        // XXX: do we need it??
        mFrame.release();
    }

    public Mat onCameraFrame(CvCameraViewFrame inputFrame) {

        Log.d(L.TAG, "main:oncamera frame");

        mFrame = inputFrame.rgba();

        if (mNativeDetector != null) {
            mNativeDetector.process(mFrame);
        }

        return mFrame;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        mCaptureMenu = menu.add(R.string.capture_marker);
        mCaptureBlueMenu = menu.add(R.string.capture_blue);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (mCaptureMenu == item) {
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
}
