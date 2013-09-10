#include <opencv2/opencv.hpp>
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

    VideoCapture capture(0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, IN_WIDTH);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, IN_HEIGHT);
    Mat frame;
    Mat dst(WIDTH, HEIGHT, CV_8UC(3));
    
    time(&start);
    long long counter=0L;
    int c;
    
    namedWindow(APP_NAME_DRAW,0);
    
    while (1) {
        capture >> frame;
        tracker->process(frame, dst);

        imshow(APP_NAME_DRAW, dst);
        //setWindowProperty(APP_NAME_DRAW, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
        //cvResizeWindow(APP_NAME_DRAW, WIDTH, HEIGHT);

        //save each frame image
        /*
        char outfile[256];
        sprintf(outfile, "/workspace/N3/images/track%03d.jpg", fileIndex++);
        int res = cvSaveImage(outfile, dst);
        printf("save to %s\n", outfile);
         */
        
        // release

        c = waitKey(2);
        if (c == '\x1b') {
            break;
        }

        // bench mark
        time(&end);
        ++counter;
        double sec = difftime(end, start);
        double fps = counter / sec;
        printf("FPS = %.2f\n", fps);

    }
    
    return 0;
}
