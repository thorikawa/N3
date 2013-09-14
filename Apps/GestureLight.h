#ifndef _GESTURE_LIGHT_H_
#define _GESTURE_LIGHT_H_

#include <opencv2/opencv.hpp>
#include <vector>
//#include <Box2D/Box2D.h>
#include "../DollarRecognizer/GeometricRecognizer.h"
#include "../N3.h"

using namespace DollarRecognizer;
using namespace std;

namespace Apps
{
    class GestureLight {
    private:
        GeometricRecognizer g;
        Path2D path;
        //b2World* world;
        time_t lastTime;
    public:
        GestureLight();
        void trackMarker (cv::Mat& destImg, cv::Point r, cv::Point b, cv::Point g, cv::Point y);
    };
  
    class GestureLightResult {
    public:
        int code;
        GestureLightResult (int code) {
            this->code = code;
        }
    };
}

#endif