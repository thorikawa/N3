#ifndef OBJECT_DETECTOR_H_
#define OBJECT_DETECTOR_H_

#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

class ObjectDetector
{
public:
  
  ObjectDetector();
  ~ObjectDetector();
  CvPoint* detect (IplImage* destImage);
  void loadSourceImage (IplImage* grayImage1);
  
private:
  const static double THRESHOLD = 0.3;
  int srcWidth, srcHeight;
  CvSeq *srcKeyPoints;
  CvSeq *srcDescriptors;
  CvMemStorage* srcStorage;
  
  int nearestNeighbor(float* vec, int laplacian, CvSeq* keypoints, CvSeq* descriptors);
  void findPairs (CvSeq* srcKeyPoints, CvSeq* srcDescriptors,
                                  CvSeq* keypoints2, CvSeq* descriptors2,
                                  vector<CvPoint2D32f>& srcPoints, vector<CvPoint2D32f>& dstPoints);
};

#endif /* OBJECT_DETECTOR_H_ */
