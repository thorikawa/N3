#include "Gesture1.h"
#include <cmath>

#define KEEP_MAX 60
#define KEEP_COUNT 5
#define NEAR_THRESHOLD 50
#define DONT_MOVE_THRESHOLD_RATE 0.8
#define SCORE_THRESHOLD 0.85

inline Point2D toPoint2D (CvPoint p) {
	return Point2D(p.x, p.y);
}

inline CvPoint toCvPoint (Point2D p) {
	return cvPoint(p.x, p.y);
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
		Point2D r = toPoint2D(_r);

		// if marker is not moving for a while, reset the path
		int len = path.size();
		if (len > KEEP_MAX) {
			path.erase(path.begin());
		}
		int nearCount = 0;
		int actual = min(KEEP_COUNT, len);

		for(int i=0; i<actual; i++){
			Point2D p = path[len-1-i];
			double d = dist(p, r);
			//printf("dist=%f\n", d);
			if (d < NEAR_THRESHOLD) ++nearCount;
		}
		if (nearCount > (double)actual * DONT_MOVE_THRESHOLD_RATE) {
			// marker is not moving, so clear the path
			printf("cleared\n");
			path.clear();
		}

		path.push_back(r);
		RecognitionResult res = g.recognize(path);
		printf("%s:%f\n", res.name.c_str(), res.score);
		if (res.name == "Circle" && res.score > SCORE_THRESHOLD) {
			printf("##circle detect##\n");
			// convert to vector<Point2D> to CvSeq<CvPoint>
			len = path.size();
			CvSeqWriter writer;
			CvMemStorage* storage = cvCreateMemStorage(0);
			cvStartWriteSeq( CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage, &writer);
			for (int i=0; i<len; i++) {
				CvPoint pt = toCvPoint(path[i]);
				CV_WRITE_SEQ_ELEM(pt, writer);
			}
			CvSeq* seq = cvEndWriteSeq(&writer);
			CvBox2D ellipse = cvFitEllipse2(seq);
			boxes.push_back(ellipse);
			cvEllipseBox(destImg, ellipse, CV_RGB(125,125,255));
		}
	}
}