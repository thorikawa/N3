#ifndef _DRAW_H_
#define _DRAW_H_

#include <opencv2/opencv.hpp>

using namespace DollarRecognizer;

namespace Apps
{
	class Draw
	{
	private:
	public:
		Draw();
		void trackMarker (IplImage *destImg, CvPoint r, CvPoint b, CvPoint g, CvPoint y);
	};	
}

#endif