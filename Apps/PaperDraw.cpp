#include "PaperDraw.h"
#include "../N3.h"

namespace Apps
{  
  PaperDraw::PaperDraw ():notFindCount(0),prevfind(false),color(CV_RGB(255,255,255)),phase(0),skipCount(0) {
    chessboard = cvLoadImage("chessboard.jpg");
    storage = (double*)malloc(sizeof(double)*9);
    homography = cvMat(3, 3, CV_64F, storage);
    wRatio = (double)WIDTH / (double)IN_WIDTH;
    hRatio = (double)HEIGHT / (double)IN_HEIGHT;
  }
  
  void PaperDraw::trackMarker (IplImage* inputImage, IplImage* destImage, CvPoint _r, CvPoint _b, CvPoint _g, CvPoint _y) {
    //cvFlip(inputImage, NULL, 1);
    switch (phase) {
      case 0: {
        // display chess board
        int cw = chessboard->width;
        int ch = chessboard->height;
        int offx = (WIDTH - cw) / 2;
        int offy = (HEIGHT - ch) / 2;
        int tw   = cw;
        int th   = ch;
        //printf("%d %d %d %d\n", offx, offy, tw, th);
        cvSetImageROI(destImage, cvRect(offx, offy, tw, th));
        cvCopy(chessboard, destImage);
        cvResetImageROI(destImage);

        // start calibration
        int cornerCount;
        //int cornerFlag = CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_NORMALIZE_IMAGE | CV_CALIB_CB_FILTER_QUADS;
        int cornerFlag = 0;
        CvSize patternSize = cvSize(CORNER_COLUMN, CORNER_ROW);
        CvPoint2D32f* corners = (CvPoint2D32f*)cvAlloc(sizeof(CvPoint2D32f) * CORNER_COUNT);
        int ret = cvFindChessboardCorners(inputImage, patternSize, corners, &cornerCount);
        printf("chess ret=%d corners=%d\n", ret, cornerCount);
        if (ret) {
          if (++skipCount < 5) {
            return;
          }
          //cvDrawChessboardCorners(destImage, patternSize, corners, cornerCount, ret);
          // notmalize
          for (int i=0; i<CORNER_COUNT; i++) {
            corners[i].x *= wRatio;
            corners[i].y *= hRatio;
          }
          CvPoint2D32f* srcPoints = (CvPoint2D32f*)cvAlloc(sizeof(CvPoint2D32f) * 4);
          CvPoint2D32f* dstPoints = (CvPoint2D32f*)cvAlloc(sizeof(CvPoint2D32f) * 4);
          srcPoints[0] = cvPoint2D32f(offx+71. , offy+73.);
          srcPoints[1] = cvPoint2D32f(offx+246., offy+73.);
          srcPoints[2] = cvPoint2D32f(offx+71. , offy+205.);
          srcPoints[3] = cvPoint2D32f(offx+246., offy+205.);
          dstPoints[0] = corners[0];
          dstPoints[1] = corners[CORNER_COLUMN-1];
          dstPoints[2] = corners[(CORNER_ROW-1)*CORNER_COLUMN];
          dstPoints[3] = corners[CORNER_COUNT-1];
          for (int i=0; i<4; i++) {
            printf("corner[%d] = (%f, %f)\n", i, dstPoints[i].x, dstPoints[i].y);
          }
          
          CvMat srcMat = cvMat(1, 4, CV_32FC2, srcPoints);
          CvMat dstMat = cvMat(1, 4, CV_32FC2, dstPoints);
          
          //if (!cvFindHomography(&srcMat, &dstMat, &homography, CV_RANSAC, 5)) {
          if (!cvFindHomography(&dstMat, &srcMat, &homography, CV_RANSAC, 5)) {
            printf("error findHomography\n");
            break;
          }


          cvSet (destImage, cvScalarAll(0), 0);

          /*
          CvPoint d = this->convertCoordinates(cvPoint((int)(corners[0].x), (int)(corners[0].y)));
          printf("draw1 (%d, %d)\n", d.x, d.y);
          cvCircle(destImage, d, 3, CV_RGB(255,0,0), -1);

          CvPoint d2 = this->convertCoordinates(cvPoint((int)(corners[CORNER_COLUMN-1].x), (int)(corners[CORNER_COLUMN-1].y)));
          printf("draw2 (%d, %d)\n", d2.x, d2.y);
          cvCircle(destImage, d2, 3, CV_RGB(0,255,0), -1);

          CvPoint d3 = this->convertCoordinates(cvPoint((int)(corners[CORNER_COUNT-1].x), (int)(corners[CORNER_COUNT-1].y)));
          printf("draw2 (%d, %d)\n", d3.x, d3.y);
          cvCircle(destImage, d3, 3, CV_RGB(0,0,255), -1);
           */

          CvPoint p1 = this->convertCoordinates(cvPoint(0,0));
          CvPoint p2 = this->convertCoordinates(cvPoint(WIDTH,0));
          CvPoint p3 = this->convertCoordinates(cvPoint(WIDTH,HEIGHT));
          CvPoint p4 = this->convertCoordinates(cvPoint(0,HEIGHT));

          cvLine(destImage, p1, p2, CV_RGB(0,255,0));
          cvLine(destImage, p2, p3, CV_RGB(0,255,0));
          cvLine(destImage, p3, p4, CV_RGB(0,255,0));
          cvLine(destImage, p4, p1, CV_RGB(0,255,0));

          phase=1;
        }
      }
      break;
      case 1: {
        // show something
#if DEBUB
        CvPoint p1 = this->convertCoordinates(cvPoint(0,0));
        CvPoint p2 = this->convertCoordinates(cvPoint(WIDTH,0));
        CvPoint p3 = this->convertCoordinates(cvPoint(WIDTH,HEIGHT));
        CvPoint p4 = this->convertCoordinates(cvPoint(0,HEIGHT));
        
        cvLine(destImage, p1, p2, CV_RGB(0,255,0));
        cvLine(destImage, p2, p3, CV_RGB(0,255,0));
        cvLine(destImage, p3, p4, CV_RGB(0,255,0));
        cvLine(destImage, p4, p1, CV_RGB(0,255,0));
#endif
        if (_r.x >= 0 && _b.x < 0) {
          // find only red
          //cvRectangle(dst, cvPoint(p.x-2, p.y-2), cvPoint(p.x+2, p.y+2), color, 3);
          CvPoint d = this->convertCoordinates(_r);
          printf("draw (%d, %d)\n", d.x, d.y);
          //cvCircle(destImage, d, 3, CV_RGB(255,0,0), -1);
          if (prevfind){
            cvLine(destImage, prevPoint, d, color, THICKNESS);
          } else {
            // if there is no prev point, do nothing
          }
          prevPoint = d;
          prevfind = true;
          notFindCount=0;
        } else {
          if(++notFindCount >= ALLOW_ERROR_COUNT) {
            prevfind = false;
          }
        }
      }
      break;
      default:
      break;
    }
  }
  
  CvPoint PaperDraw::convertCoordinates (CvPoint p) {
    CvMat *src = cvCreateMat(3, 1, CV_64F);
    CvMat *dst = cvCreateMat(3, 1, CV_64F);
    cvmSet(src, 0, 0, 1.*p.x);
    cvmSet(src, 1, 0, 1.*p.y);
    cvmSet(src, 2, 0, 1.);
    cvMatMul(&homography, src, dst);
    double x = CV_MAT_ELEM(*dst, double, 0, 0);
    double y = CV_MAT_ELEM(*dst, double, 1, 0);
    double z = CV_MAT_ELEM(*dst, double, 2, 0);
    printf("%f %f %f\n", x, y, z);
    return cvPoint((int)(x/z), (int)(y/z));
  }
}