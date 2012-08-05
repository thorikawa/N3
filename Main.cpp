#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "Apps/Draw.h"
#include "Apps/Gesture1.h"
#include "Apps/Gunman.h"
#include "Apps/PaperDraw.h"
#include "N3.h"

#define AREA_THRESHOLD 30
#define APP_NAME_DRAW "draw"

using namespace Apps;

/**
 * get HS only Histogram from IplImage
 */
CvHistogram* getHSHist (IplImage* img, CvRect rect) {
	cvSetImageROI(img, rect);
	
	// Compute HSV image and separate into colors
	IplImage* hsv = cvCreateImage(cvGetSize(img), IPL_DEPTH_8U, 3);
	
	cvCvtColor(img, hsv, CV_BGR2HSV);
	
	IplImage* h_plane = cvCreateImage(cvGetSize( img ), 8, 1);
	IplImage* s_plane = cvCreateImage(cvGetSize( img ), 8, 1);
	IplImage* v_plane = cvCreateImage(cvGetSize( img ), 8, 1);
	IplImage* planes[] = { h_plane, s_plane };
	cvCvtPixToPlane(hsv, h_plane, s_plane, v_plane, 0);
	
	// Build and fill the histogram
	int h_bins = 30, s_bins = 32;
	CvHistogram* hist;
	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
	cvCalcHist(planes, hist, 0, 0); // Compute histogram
	cvNormalizeHist(hist, 20*255); // Normalize it
	
	cvReleaseImage(&h_plane);
	cvReleaseImage(&s_plane);
	cvReleaseImage(&v_plane);
	cvResetImageROI(img);
	
	return hist;
}

/**
 * find rectangle which represents the marker by histogram from image
 */
CvRect findMarker (IplImage* img, IplImage** frame_planes, CvHistogram* hist, int* find) {
	IplImage* back_img = cvCreateImage(cvGetSize(frame_planes[0]), IPL_DEPTH_8U, 1);
	cvCalcBackProject(frame_planes, back_img, hist);// Calculate back projection
	
	// do we need it?
	//cvNormalizeHist(hist, 1.0); // Normalize it	
	
	cvThreshold(back_img,back_img,30,255,CV_THRESH_BINARY);
	
	//cvMorphologyEx(back_img, back_img, 0, 0, CV_MOP_OPEN);
	//cvMorphologyEx(back_img, back_img, 0, 0, CV_MOP_CLOSE);
	cvSmooth(back_img, back_img);
	
	CvMemStorage *storage = cvCreateMemStorage ();
	CvSeq *contours = 0;
	cvFindContours(back_img, storage, &contours, sizeof(CvContour), CV_RETR_EXTERNAL);
	
	//Find max contours rect		
	CvSeq *maxContour;
	CvRect maxRect;
	double maxArea = 0.0F;
	for (; contours != 0; contours = contours->h_next)
	{
		CvRect rect = cvBoundingRect(contours);
#if DEBUG
    //cvDrawContours(img, contours, CV_RGB(255,0,0), CV_RGB(255,0,0), 0, 2);
#endif
		//printf("draw bounding: %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
		double area = cvContourArea(contours);
		if (area > maxArea) {
			maxContour = contours;
			maxRect = rect;
			maxArea = area;
		}
		//printf("area: %f\n", area);
	}
	
	cvReleaseImage(&back_img);
	
	if (maxArea > AREA_THRESHOLD) {
		//printf("find! %f\n", maxArea);
		*find = 1;
	} else {
		//printf("not find... %f\n", maxArea);
		*find = 0;
	}
	return maxRect;
}

/**
 * find center of CvRect
 */
inline CvPoint center (CvRect rect) {
	return cvPoint(rect.x+rect.width/2, rect.y+rect.height/2);
}

/**
 * Main
 */
int main (int argc, char* argv[]) {
	// Set up images
	//IplImage* img = cvLoadImage("test0605.jpg");
	IplImage* img = cvLoadImage("rby0610.jpg");
	//CvHistogram* hist = getHSHist(img, cvRect(84, 196, 43, 23));
	CvHistogram* rHist = getHSHist(img, cvRect(446, 294, 50, 50));
	//CvHistogram* yHist = getHSHist(img, cvRect(441, 127, 50, 50));
	CvHistogram* yHist = getHSHist(img, cvRect(532, 40, 50, 50));

	int fileIndex = 0;
	
	// benchmark
	time_t start, end;
	
	IplImage *dst = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
	
	//CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
	CvCapture* capture = cvCreateCameraCapture(1);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, IN_WIDTH);
	cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, IN_HEIGHT);
	IplImage *frame = 0;
	
	time(&start);
	long long counter=0L;
	int c;
	Gesture1 gesture1 = Gesture1();
  Gunman gunman = Gunman();
  Draw draw = Draw();
  PaperDraw paperDraw = PaperDraw();
	
	cvNamedWindow(APP_NAME_DRAW,0);
	
	double wRatio = (double)WIDTH / (double)IN_WIDTH;
	double hRatio = (double)HEIGHT / (double)IN_HEIGHT;
	
	CvSize captureSize = cvSize(IN_WIDTH, IN_HEIGHT);
	IplImage* frame_hsv = cvCreateImage(captureSize, IPL_DEPTH_8U, 3);
	IplImage* frame_h = cvCreateImage(captureSize, 8, 1);
	IplImage* frame_s = cvCreateImage(captureSize, 8, 1);
	IplImage* frame_v = cvCreateImage(captureSize, 8, 1);
	IplImage* frame_planes[] = { frame_h, frame_s };
	
	while (1) {
		frame = cvQueryFrame(capture);
		//cvFlip (frame, frame, 1);
		//dst = frame;
#if DEBUG
		cvResize(frame, dst);
#endif
		
		cvCvtColor(frame, frame_hsv, CV_BGR2HSV);
		cvCvtPixToPlane( frame_hsv, frame_h, frame_s, frame_v, 0 );

		int rFind = 0;
		int yFind = 0;
		CvRect rRect = findMarker(dst, frame_planes, rHist, &rFind);
		//CvRect yRect = findMarker(dst, frame_planes, yHist, &yFind);
    CvRect yRect = cvRect(0,0,0,0);
		
		printf("R=%d Y=%d\n", rFind, yFind);
		
		CvPoint rc = center(rRect);
		rc.x = rc.x * wRatio; rc.y = rc.y * hRatio;
		CvPoint yc = center(yRect);
		yc.x = yc.x * wRatio; yc.y = yc.y * hRatio;

		if(!rFind) rc.x = -1;
		if(!yFind) yc.x = -1;
#if DEBUG
		if(rFind) cvRectangle(dst, cvPoint(rc.x-2, rc.y-2), cvPoint(rc.x+2, rc.y+2), CV_RGB(255,0,0), 3);
		if(yFind) cvRectangle(dst, cvPoint(yc.x-2, yc.y-2), cvPoint(yc.x+2, yc.y+2), CV_RGB(0,255,0), 3);
#endif

		//gesture1.trackMarker(dst, rc, yc, cvPoint(0,0), cvPoint(0,0));
    //gunman.trackMarker(dst, rc, yc, cvPoint(0,0), cvPoint(0,0));
    //draw.trackMarker(dst, rc, yc, cvPoint(0,0), cvPoint(0,0));
    paperDraw.trackMarker(frame, dst, rc, yc, cvPoint(0,0), cvPoint(0,0));

		cvShowImage(APP_NAME_DRAW, dst);
		cvSetWindowProperty(APP_NAME_DRAW, CV_WND_PROP_FULLSCREEN,CV_WINDOW_FULLSCREEN);
		//cvResizeWindow(APP_NAME_DRAW, WIDTH, HEIGHT);

		//save each frame image
    /*
    char outfile[256];
    sprintf(outfile, "/workspace/N3/images/track%03d.jpg", fileIndex++);
    int res = cvSaveImage(outfile, dst);
    printf("save to %s\n", outfile);
     */
		
		// release

		c = cvWaitKey(2);
		if (c == '\x1b')
			break;

		// bench mark
		time(&end);
		++counter;
		double sec = difftime (end, start);
		double fps = counter / sec;
		printf("FPS = %.2f\n", fps);

	}

	cvReleaseImage(&frame_hsv);
	cvReleaseImage(&frame_h);
	cvReleaseImage(&frame_s);
	cvReleaseImage(&frame_v);
	
	cvReleaseCapture(&capture);
	
	return 0;
}
