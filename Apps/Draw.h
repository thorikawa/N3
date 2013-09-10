#ifndef _DRAW_H_
#define _DRAW_H_

#include <opencv2/opencv.hpp>

using namespace cv;

namespace Apps
{
	class Draw
	{
	private:
    const static int ALLOW_ERROR_COUNT = 5;
    const static int THICKNESS = 2;
    int notFindCount;
    bool prevfind;
    Point prevPoint;
    Scalar color;
	public:
		Draw();
		void trackMarker (Mat& destImg, Point& r, Point& b, Point& g, Point& y);
	};	
}

#endif