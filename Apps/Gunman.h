#ifndef _GUNMAN_H_
#define _GUNMAN_H_

#include <opencv2/opencv.hpp>
#include "../N3.h"

using namespace std;

namespace Apps
{
  class Gunman
  {
  private:
  public:
    Gunman();
    void trackMarker (IplImage* destImg, CvPoint r, CvPoint b, CvPoint g, CvPoint y);
  };
}

#endif