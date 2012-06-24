#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/nonfree/nonfree.hpp>

using namespace std;

const int DIM_VECTOR = 128;

/**
 * Dump keypoints to file
 * copy from http://d.hatena.ne.jp/aidiary/20091030/1256905218 and modified by thorikawa
 * @param imageDescriptors SURF feature descriptors
 */
void writeSURF(CvSeq* imageDescriptors) {
  for (int i = 0; i < imageDescriptors->total; i++) {
    float* descriptor = (float*)cvGetSeqElem(imageDescriptors, i);
    for (int j = 0; j < DIM_VECTOR; j++) {
      printf("%f\t", descriptor[j]);
    }
    cout << endl;
  }
}

int main(int argc, char** argv) {
  cv::initModule_nonfree();
  if (argc < 1) {
  cerr << "usage:" << endl;
  cerr << " a.out example.jpg > output.tsv" << endl;
  return -1;
  }

  const char* imageFile = argc == 2 ? argv[1] : "image/accordion_image_0001.jpg";

  IplImage* grayImage = cvLoadImage(imageFile, CV_LOAD_IMAGE_GRAYSCALE);
  if (!grayImage) {
    cerr << "cannot find image file: " << imageFile << endl;
    return -1;
  }

  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSeq* imageKeypoints = 0;
  CvSeq* imageDescriptors = 0;
  CvSURFParams params = cvSURFParams(500, 1);

  cvExtractSURF(grayImage, 0, &imageKeypoints, &imageDescriptors, storage, params);
  cerr << "Image Descriptors: " << imageDescriptors->total << endl;

  writeSURF(imageDescriptors);

  cvReleaseImage(&grayImage);
  cvClearSeq(imageKeypoints);
  cvClearSeq(imageDescriptors);
  cvReleaseMemStorage(&storage);

  return 0;
}
