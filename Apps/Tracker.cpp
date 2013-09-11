#include "Tracker.h"
#define AREA_THRESHOLD 30

namespace Apps
{
    /**
     * get HS only Histogram from IplImage
     */
    void getHSHist(Mat& img, MatND& hist) {
        // Compute HSV image and separate into colors
        Mat hsv;
        cvtColor(img, hsv, CV_BGR2HSV);

        // copied the code from fancy demo linked from here:
        // http://docs.opencv.org/doc/tutorials/imgproc/histograms/back_projection/back_projection.html
        // Build and fill the histogram
        int hbins = 30, sbins = 32;
        int histSize[] = { hbins, sbins };
        float hranges[] = { 0, 179 };
        float sranges[] = { 0, 255 };
        const float* ranges[] = { hranges, sranges };
        int channels[] = {0, 1};
        calcHist(&hsv, 1, channels, Mat(), // do not use mask
                 hist, 2, histSize, ranges);
        normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat());
        
        return;
    }

    /**
     * find rectangle which represents the marker by histogram from image
     */
    Rect findMarker (Mat& img, MatND& hist, int* find) {
        Mat hsv;
        cvtColor(img, hsv, CV_BGR2HSV);
        Mat backProject(hsv.size(), CV_8UC1);
        float hranges[] = { 0, 179 };
        float sranges[] = { 0, 255 };
        const float* ranges[] = { hranges, sranges };
        int channels[] = {0, 1};
        calcBackProject(&hsv, 1, channels, hist, backProject, ranges, 1, true);// Calculate back projection
        
        // do we need it?
        //cvNormalizeHist(hist, 1.0); // Normalize it
        
        threshold(backProject, backProject, 30, 255, CV_THRESH_BINARY);
        
        //cvMorphologyEx(back_img, back_img, 0, 0, CV_MOP_OPEN);
        //cvMorphologyEx(back_img, back_img, 0, 0, CV_MOP_CLOSE);
        
        // XXX: do we need it?
        //GaussianBlur(backProject, backProject, backProject.size());
        
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        findContours(backProject, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
        
        //Find max contours rect
        Rect maxRect;
        double maxArea = 0.0F;
        for (int i = 0; i < contours.size(); i++) {
            Rect rect = boundingRect(Mat(contours[i]));
#if DEBUG
            drawContours(img, contours, i, Scalar(255, 255, 255), 2, 8, hierarchy, 0, Point(0, 0));
#endif
            // printf("draw bounding: %d %d %d %d\n", rect.x, rect.y, rect.width, rect.height);
            double area = contourArea(contours[i]);
            if (area > maxArea) {
                maxRect = rect;
                maxArea = area;
            }
            //printf("area: %f\n", area);
        }
        
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
    inline Point center (Rect rect) {
        return Point(rect.x+rect.width/2, rect.y+rect.height/2);
    }

    void Tracker::init () {
        wRatio = (double)WIDTH / (double)IN_WIDTH;
        hRatio = (double)HEIGHT / (double)IN_HEIGHT;

        //Gesture1 gesture1 = Gesture1();
        draw = new Draw();
        //PaperDraw paperDraw = PaperDraw();
    }

    Tracker::Tracker (string imageFileName) {
        Mat image = imread(imageFileName);
        Mat rImage = image(Rect(446, 294, 50, 50));
        Mat yImage = image(Rect(532, 40, 50, 50));
        getHSHist(rImage, rHist);
        getHSHist(yImage, yHist);

        init();
    }

    Tracker::Tracker(string rImageFile, string yImageFile)  {
        Mat rImage = imread(rImageFile);
        Mat yImage = imread(yImageFile);
        getHSHist(rImage, rHist);
        getHSHist(yImage, yHist);

        init();
    }

    Tracker::~Tracker() {
    }

    void Tracker::process (Mat& src, Mat& dst) {
        //cvFlip (frame, frame, 1);
        //dst = frame;

        //src.copyTo(dst);
        resize(src, dst, Size(WIDTH, HEIGHT));


        int rFind = 0;
        int yFind = 0;
        Rect rRect = findMarker(src, rHist, &rFind);
        //CvRect yRect = findMarker(dst, frame_planes, yHist, &yFind);
        Rect yRect = Rect(0,0,0,0);

        printf("R=%d Y=%d\n", rFind, yFind);

        Point rc = center(rRect);
        rc.x = rc.x * wRatio; rc.y = rc.y * hRatio;
        Point yc = center(yRect);
        yc.x = yc.x * wRatio; yc.y = yc.y * hRatio;

        if(!rFind) rc.x = -1;
        if(!yFind) yc.x = -1;
#if DEBUG
        if(rFind) rectangle(dst, Point(rc.x-2, rc.y-2), Point(rc.x+2, rc.y+2), CV_RGB(255,0,0), 3);
        if(yFind) rectangle(dst, Point(yc.x-2, yc.y-2), Point(yc.x+2, yc.y+2), CV_RGB(0,255,0), 3);
#endif

        Point zp = Point(0,0);
        //gesture1.trackMarker(dst, rc, yc, cvPoint(0,0), cvPoint(0,0));
        //gunman.trackMarker(dst, rc, yc, cvPoint(0,0), cvPoint(0,0));
        draw->trackMarker(dst, rc, yc, zp, zp);
        //paperDraw.trackMarker(frame, dst, rc, yc, cvPoint(0,0), cvPoint(0,0));
    }
}