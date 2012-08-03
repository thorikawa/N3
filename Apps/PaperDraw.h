#ifndef _PAPERDRAW_H_
#define _PAPERDRAW_H_

#include <opencv2/opencv.hpp>

namespace Apps
{
	class PaperDraw
	{
	private:
    const static int ALLOW_ERROR_COUNT = 5;
    const static int THICKNESS = 2;
    const static int CORNER_ROW = 4;
    const static int CORNER_COLUMN = 5;
    const static int CORNER_COUNT = CORNER_ROW * CORNER_COLUMN;
    int phase;
    int notFindCount;
    bool prevfind;
    CvPoint prevPoint;
    CvScalar color;
    IplImage *chessboard;
    double *storage;
    CvMat homography;
    double wRatio, hRatio;
	public:
		PaperDraw();
		void trackMarker (IplImage* inputImage, IplImage *destImg, CvPoint r, CvPoint b, CvPoint g, CvPoint y);
    CvPoint convertCoordinates (CvPoint p);
	};	
}

#endif