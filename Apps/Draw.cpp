#include "Draw.h"

namespace Apps
{	
	Draw::Draw () {
	}
	
	void Draw::trackMarker (IplImage* destImg, CvPoint _r, CvPoint _b, CvPoint _g, CvPoint _y) {
		if (rFind && !yFind) {
			// find red
			CvPoint p = center(rRect);
			p.x = p.x * wRatio;
			p.y = p.y * hRatio;
			//cvRectangle(dst, cvPoint(p.x-2, p.y-2), cvPoint(p.x+2, p.y+2), color, 3);
			if (prevfind){
				cvLine(dst, prevPoint, p, color, THICKNESS);
			} else {
				// if there is no prev point, do nothing
			}
			prevPoint = p;
			prevfind = true;
			notFindCount=0;
			
		} else {
			if(++notFindCount >= ALLOW_ERROR_COUNT) {
				prevfind = false;
			}
		}		
	}
}