#include "ObjectRecognizer.h"

using namespace cv;

bool ObjectRecognizer::loadDescription (const char* filename) {
  
  printf("load desc: %s\n", filename);
  
  ifstream descFile(filename);
  if (descFile.fail()) {
    printf("file open error\n");
    return false;
  }
  
  int num = 0;
  string line;
  while (getline(descFile, line, '\n')) {
    num++;
  }
  CvMat* objMat = cvCreateMat(num, DIM, CV_32FC1);
  descFile.clear();
  descFile.seekg(0);
  
  int cur = 0;
  while (getline(descFile, line, '\n')) {
    vector<string> ldata;
    istringstream ss(line);
    string s;
    while (getline(ss, s, '\t')) {
      ldata.push_back(s);
    }
    for (int i=0; i<DIM; i++) {
      float val = atof(ldata[i].c_str());
      CV_MAT_ELEM(*objMat, float, cur, i) = val;
    }
    cur++;
  }
  descFile.close();
  
  cvLSHAdd(lsh, objMat);
  
  objNumber++;
  lastIndex += objMat->rows;
  lastIndexes.push_back(lastIndex);
  numKeypoints.push_back(objMat->rows);
  
  printf("load row: %d\n", cur);
  
  
  return true;
}

ObjectRecognizer::ObjectRecognizer () :
objNumber(0),
lastIndex(-1)
{
  initModule_nonfree();
  lsh = cvCreateMemoryLSH(DIM, 100000, 5, 64, CV_32FC1);
}

ObjectRecognizer::~ObjectRecognizer () {
  // TODO Auto-generated destructor stub
}

int ObjectRecognizer::getObjectId (int index) {
  int i;
  for (i=0; i<objNumber; i++) {
    if (index <= lastIndexes[i]) {
      break;
    }
  }
  return i;
}

int ObjectRecognizer::detect (IplImage* queryImage) {
  //printf("Detect Feature Start\n");
  
  if (queryImage == NULL)
    return -1; //no image at input_idx!
  
  // extract feature description from the query image
  CvSeq *queryKeypoints = 0;
  CvSeq *queryDescriptors = 0;
  CvMemStorage *storage = cvCreateMemStorage(0);
  CvSURFParams params = cvSURFParams(500, 1);
  cvExtractSURF(queryImage, 0, &queryKeypoints, &queryDescriptors, storage, params);
  //printf("%d keypoints detected\n", queryDescriptors->total);
  
  // set up voting box
  vector<int> flag(LSHSize(lsh));
  vector<int> votes(objNumber);
  
  // convert features of the query image into CvMat
  CvMat* queryMat = cvCreateMat(queryDescriptors->total, DIM, CV_32FC1);
  CvSeqReader reader;
  float* ptr = queryMat->data.fl;
  cvStartReadSeq(queryDescriptors, &reader);
  for (int i = 0; i < queryDescriptors->total; i++) {
    float* descriptor = (float*)reader.ptr;
    CV_NEXT_SEQ_ELEM(reader.seq->elem_size, reader);
    memcpy(ptr, descriptor, DIM * sizeof(float));
    ptr += DIM;
  }
  
  // find 1-NN keypoints index using LSH
  int k = 1;
  CvMat* indices = cvCreateMat(queryKeypoints->total, k, CV_32SC1);
  CvMat* dists = cvCreateMat(queryKeypoints->total, k, CV_64FC1);
  cvLSHQuery(lsh, queryMat, indices, dists, k, 100);
  cvReleaseMat(&queryMat);
  
  // vote the object including 1-NN keypoints
  //printf("check %d nns\n", indices->rows);
  for (int i = 0; i < indices->rows; i++) {
    int idx = CV_MAT_ELEM(*indices, int, i, 0);
    double dist = CV_MAT_ELEM(*dists, double, i, 0);
    if (idx < 0) {
      // can't find nn
      continue;
    }
    
    if (!flag[idx] && dist < THRESHOLD) {
      int id = getObjectId(idx);
      votes[id]++;
      flag[idx] = 1;
    }
  }  
  //printf("end: find nn\n");
  
  // find who's hotest
  int maxId = -1;
  int maxVal = -1;
  for (int i = 0; i < objNumber; i++) {
    printf("vote for %d: %d\n", i, votes[i]);    
    if (votes[i] > maxVal) {
      maxId = i;
      maxVal = votes[i];
    }
  }
  printf("this object is %d\n", maxId);
  
  // benchmark
  // tt = (double)cvGetTickCount() - tt;
  // cout << "Recognition Time = " << tt / (cvGetTickFrequency() * 1000.0) << "ms" << endl;
  
  if (maxVal > numKeypoints[maxId]/10) {
    for (int i = 0; i < indices->rows; i++) {
      int idx = CV_MAT_ELEM(*indices, int, i, 0);
      double dist = CV_MAT_ELEM(*dists, double, i, 0);
      if (idx < 0) {
        // can't find nn
        continue;
      }
      
      if (dist < THRESHOLD) {
        int id = getObjectId(idx);
        if (id == maxId) {
          CvSURFPoint* spt = (CvSURFPoint*)cvGetSeqElem(queryKeypoints, i);
          //circle(img, spt->pt, 3, cvScalar(255, 0, 255, 0));
        }
      }
    }
  } else {
    maxId = -1;
  }
  
  cvClearSeq(queryKeypoints);
  cvClearSeq(queryDescriptors);
  cvReleaseMemStorage(&storage);
  
  return maxId;
}
