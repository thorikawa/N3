#include "Draw.h"

namespace Apps
{	
	Draw::Draw ():notFindCount(0),prevfind(false),color(CV_RGB(255,255,255)) {
	}
	
	void Draw::trackMarker (IplImage* destImg, CvPoint _r, CvPoint _b, CvPoint _g, CvPoint _y) {
		if (_r.x >= 0 && _b.x < 0) {
			// find only red
			//cvRectangle(dst, cvPoint(p.x-2, p.y-2), cvPoint(p.x+2, p.y+2), color, 3);
			if (prevfind){
				cvLine(destImg, prevPoint, _r, color, THICKNESS);
			} else {
				// if there is no prev point, do nothing
			}
			prevPoint = _r;
			prevfind = true;
			notFindCount=0;
		} else {
			if(++notFindCount >= ALLOW_ERROR_COUNT) {
				prevfind = false;
			}
		}		
	}
}