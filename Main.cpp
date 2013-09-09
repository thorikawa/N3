#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "Apps/Tracker.h"
#include "N3.h"

#define APP_NAME_DRAW "draw"

using namespace Apps;

/**
 * Main
 */
int main (int argc, char* argv[]) {
    // Set up images
    Tracker* tracker = new Tracker("rby0610.jpg");

    int fileIndex = 0;

    // benchmark
    time_t start, end;
    
    //CvCapture* capture = cvCreateCameraCapture(CV_CAP_ANY);
    CvCapture* capture = cvCreateCameraCapture(0);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, IN_WIDTH);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, IN_HEIGHT);
    IplImage *frame = 0;
    IplImage *dst = cvCreateImage(cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);
    
    time(&start);
    long long counter=0L;
    int c;
    
    cvNamedWindow(APP_NAME_DRAW,0);
    
    while (1) {
        frame = cvQueryFrame(capture);
        tracker->process(frame, dst);

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
        if (c == '\x1b') {
            break;
        }

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
