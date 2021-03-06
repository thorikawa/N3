#ifndef _GESTURE1_H_
#define _GESTURE1_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <Box2D/Box2D.h>
#include "../DollarRecognizer/GeometricRecognizer.h"
#include "../ObjectDetector.h"
#include "../N3.h"

using namespace DollarRecognizer;
using namespace std;

namespace Apps
{
  class Gesture1
  {
  private:
    GeometricRecognizer g;
    Path2D path;
    b2World* world;
    time_t lastTime;
    ObjectDetector* objectDetector;
  public:
    Gesture1();
    void trackMarker (IplImage* destImg, CvPoint r, CvPoint b, CvPoint g, CvPoint y);
  };
  
  class Gesture1Result
  {
  public:
    int code;
    Gesture1Result (int code) {
      this->code = code;
    }
  };
}

#endif