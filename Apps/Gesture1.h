#ifndef _GESTURE1_H_
#define _GESTURE1_H_

#include <opencv2/opencv.hpp>
#include "../DollarRecognizer/GeometricRecognizer.h"

using namespace DollarRecognizer;

namespace Apps
{
	class Gesture1
	{
	private:
		GeometricRecognizer g;
		Path2D path;
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