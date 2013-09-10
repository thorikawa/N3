#ifndef _GUNMAN_H_
#define _GUNMAN_H_

#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include "../N3.h"
#include "Draw.h"

using namespace std;
using namespace cv;

namespace Apps
{
    class Tracker {
    private:
        CvHistogram* rHist;
        CvHistogram* yHist;
        IplImage* frame_hsv;
        IplImage* frame_h;
        IplImage* frame_s;
        IplImage* frame_v;
        Draw* draw;
        double wRatio;
        double hRatio;
    public:
        Tracker(string imageFileName);
        ~Tracker();
        void process (Mat &src, Mat &dst);
  };
}

#endif