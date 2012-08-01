#ifndef _DRAW_H_
#define _DRAW_H_

#include <opencv2/opencv.hpp>

namespace Apps
{
	class Draw
	{
	private:
    const static int ALLOW_ERROR_COUNT = 5;
    const static int THICKNESS = 2;
    int notFindCount;
    bool prevfind;
    CvPoint prevPoint;
    CvScalar color;
	public:
		Draw();
		void trackMarker (IplImage *destImg, CvPoint r, CvPoint b, CvPoint g, CvPoint y);
	};	
}

#endif