//#include "cv.h"
//#include "highgui.h"
#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 480
#define HEIGHT 360
#define THICKNESS 3
#define ALLOW_ERROR_COUNT 5

inline CvPoint center(CvRect rect){
	return cvPoint(rect.x+rect.width/2, rect.y+rect.height/2);
}

int main(int argc, char* argv[]){
	// Set up images
	IplImage* img = cvLoadImage("test0605.jpg");
	cvSetImageROI(img, cvRect(84, 196, 43, 23));
	
	// Compute HSV image and separate into colors
	IplImage* hsv = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );

	cvCvtColor( img, hsv, CV_BGR2HSV );
	
	IplImage* h_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* s_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* v_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* planes[] = { h_plane, s_plane };
	cvCvtPixToPlane( hsv, h_plane, s_plane, v_plane, 0 );

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

	int fileIndex = 0;
	
	// benchmark
	time_t start, end;
	
	IplImage *dst = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
	CvScalar color = CV_RGB(255,255,255);
	
	CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_WIDTH, WIDTH);
	cvSetCaptureProperty (capture, CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);	
	IplImage *frame = 0;
	
	time(&start);
	long long counter=0L;
	int c;
	bool prevfind=false;
	int notFindCount=0;
	CvPoint prevPoint;
	while (1) {
		frame = cvQueryFrame(capture);
		//frame = img;

		IplImage* frame_hsv = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 3 );
		cvCvtColor( frame, frame_hsv, CV_BGR2HSV );
		IplImage* frame_h = cvCreateImage( cvGetSize( frame ), 8, 1 );
		IplImage* frame_s = cvCreateImage( cvGetSize( frame ), 8, 1 );
		IplImage* frame_v = cvCreateImage( cvGetSize( frame ), 8, 1 );
		IplImage* frame_planes[] = { frame_h, frame_s };
		cvCvtPixToPlane( frame_hsv, frame_h, frame_s, frame_v, 0 );
		
		IplImage* back_img = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
		
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
			//printf("draw bounding: %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
			double area = cvContourArea(contours);
			if (area > maxArea) {
				maxContour = contours;
				maxRect = rect;
				maxArea = area;
			}
			//printf("area: %f\n", area);
		}
		
		if (maxArea > 200) {
			// find red
			CvPoint p = center(maxRect);
			//cvRectangle(dst, cvPoint(p.x-2, p.y-2), cvPoint(p.x+2, p.y+2), color, 3);
			if (prevfind){
				cvLine(dst, prevPoint, p, color, THICKNESS);
			} else {
				// if there is no prev point, do nothing
			}
			prevPoint = p;
			prevfind = true;
			notFindCount=0;
		} else {
			if(++notFindCount >= ALLOW_ERROR_COUNT) {
				prevfind = false;
			}
		}
		
		cvShowImage("Keypoint Matching", dst);

		//save each frame image
		/*
    char outfile[256];
    sprintf(outfile, "/Users/takahiro/Dropbox/Private/cv/track/track%03d.jpg", fileIndex++);
    int res = cvSaveImage(outfile, frame);
    printf("save to %s\n", outfile);
		 */
		
		// release
		cvReleaseImage(&frame_hsv);
		cvReleaseImage(&frame_h);
		cvReleaseImage(&frame_s);
		cvReleaseImage(&frame_v);
		cvReleaseImage(&back_img);

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
	
	cvReleaseCapture(&capture);
	
	return 0;
}
