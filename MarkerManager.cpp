#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include "MarkerManager.h"
#include "N3.h"

#define WINDOW_NAME "marker - camera"
#define PREVIEW_NAME "marker - preview"

using namespace cv;

namespace N3 {

    int drag = 0;
    int select_flag = 0;
    Point point1, point2;
    Mat frame;
    Mat cropped;

    Rect rectToCrop () {
        return Rect(point1.x, point1.y, point2.x - point1.x, point2.y - point1.y);
    }

    void MarkerManager::start (string fileName) {

        VideoCapture capture(0);
        capture.set(CV_CAP_PROP_FRAME_WIDTH, IN_WIDTH);
        capture.set(CV_CAP_PROP_FRAME_HEIGHT, IN_HEIGHT);

        namedWindow(WINDOW_NAME, 0);

        while (1) {
            capture >> frame;
            imshow(WINDOW_NAME, frame);

            char c = waitKey(2);
            if (c == '\x1b') {
                break;
            }
        }

        setMouseCallback(WINDOW_NAME, MarkerManager::mouseHandler, 0);
        while (1) {
            waitKey(0);
            if (select_flag) {
                break;
            }
        }
        imwrite(fileName, cropped);

        destroyWindow(WINDOW_NAME);
        destroyWindow(PREVIEW_NAME);

        return;
    }

    void MarkerManager::mouseHandler(int event, int x, int y, int flags, void* param) {
        if (event == CV_EVENT_LBUTTONDOWN && !drag) {
            /* left button clicked. ROI selection begins */
            point1 = Point(x, y);
            drag = 1;
            select_flag = 0;
        }
        
        if (event == CV_EVENT_MOUSEMOVE && drag) {
            /* mouse dragged. ROI being selected */
            Mat image = frame.clone();
            point2 = Point(x, y);
            rectangle(image, point1, point2, CV_RGB(255, 0, 0), LINE_WIDTH, 8, 0);
            imshow(WINDOW_NAME, image);
        }
        
        if (event == CV_EVENT_LBUTTONUP && drag) {
            Mat image = frame.clone();
            point2 = Point(x, y);
            rectangle(image, point1, point2, CV_RGB(255, 0, 0), LINE_WIDTH, 8, 0);
            imshow(WINDOW_NAME, image);

            drag = 0;

            Rect r = rectToCrop();
            if (r.width > 0 && r.height > 0) {
                cropped = frame(r);
                imshow(PREVIEW_NAME, cropped);
                select_flag = 1;
            }
        }
    }
}