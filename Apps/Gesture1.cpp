#include "Gesture1.h"
#include <cmath>

#define KEEP_COUNT 10
#define NEAR_THRESHOLD 10
#define DONT_MOVE_THRESHOLD_RATE 0.8

inline Point2D cvtPoint (CvPoint p) {
	return Point2D(p.x, p.y);
}

inline double dist (Point2D p1, Point2D p2) {
	double dx = p2.x - p1.x;
	double dy = p2.y - p1.y;
	return sqrt((dx * dx) + (dy * dy));	
}

namespace Apps
{	
	Gesture1::Gesture1 () {
		g.loadTemplates();
	}
	
	void Gesture1::trackMarker (IplImage* destImg, CvPoint _r, CvPoint _b, CvPoint _g, CvPoint _y) {
		if (_r.x < 0) return;
		Point2D r = cvtPoint(_r);
		// if marker is not moving for a while reset the path
		int len = path.size();
		int nearCount = 0;
		int actual = min(KEEP_COUNT, len);
		for(int i=0; i<actual; i++){
			Point2D p = path[len-1-i];
			double d = dist(p, r);
			if (d < NEAR_THRESHOLD) ++nearCount;
		}
		if (nearCount > (double)actual * DONT_MOVE_THRESHOLD_RATE) {
			// marker is not moving, so clear the path
			printf("cleared\n");
			path.clear();
		}
		path.push_back(r);
		RecognitionResult res = g.recognize(path);
		if (res.name == "Circle") {
			printf("##circle detect##");
		}
	}
}