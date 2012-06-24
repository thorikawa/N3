#include "ObjectDetector.h"

using namespace std;

const int DIM_VECTOR = 128;    // 128次元ベクトル
const double THRESHOLD = 0.3;  // 線をつなぐ距離の閾値

/**
 * 2つのベクトルのユークリッド距離を計算して返す
 * @param[in] vec     ベクトル1の配列
 * @param[in] mvec    ベクトル2の配列
 * @param[in] length  ベクトルの長さ
 * @return ユークリッド距離
 */
inline double euclidDistance (float* vec1, float* vec2, int length) {
  double sum = 0.0;
  for (int i = 0; i < length; i++) {
    sum += (vec1[i] - vec2[i]) * (vec1[i] - vec2[i]);
  }
  return sqrt(sum);
}

ObjectDetector::ObjectDetector () {
}

ObjectDetector::~ObjectDetector () {
  if (srcKeyPoints != NULL)
    cvClearSeq(srcKeyPoints);
  if (srcDescriptors != NULL)
    cvClearSeq(srcDescriptors);
  if (srcStorage != NULL)
    cvReleaseMemStorage(&srcStorage);
}

void ObjectDetector::loadSourceImage (IplImage* grayImage1) {
  srcWidth = grayImage1->width;
  srcHeight = grayImage1->height;
  srcStorage = cvCreateMemStorage(0);
  CvSURFParams params = cvSURFParams(500, 1);  
  cvExtractSURF(grayImage1, 0, &srcKeyPoints, &srcDescriptors, srcStorage, params);
}

/**
 * 最近傍点を検索
 *
 * @param[in]   vec             特徴ベクトル
 * @param[in]   laplacian       ラプラシアン
 * @param[in]   keypoints       キーポイントの集合（この中から最近傍点を検索）
 * @param[in]   descriptors     特徴ベクトルの集合
 *
 * @return 最近傍点のインデックス（見つからないときは-1）
 */
int ObjectDetector::nearestNeighbor(float* vec, int laplacian, CvSeq* keypoints, CvSeq* descriptors) {
  int neighbor = -1;
  double minDist = 1e6;
  
  for (int i = 0; i < descriptors->total; i++) {
    CvSURFPoint* pt = (CvSURFPoint*)cvGetSeqElem(keypoints, i);
    // ラプラシアンが異なるキーポイントは無視
    if (laplacian != pt->laplacian) continue;
    float* v = (float*)cvGetSeqElem(descriptors, i);
    double d = euclidDistance(vec, v, DIM_VECTOR);
    // より近い点があったら置き換える
    if (d < minDist) {
      minDist = d;
      neighbor = i;
    }
  }
  
  // 最近傍点でも距離が閾値以上だったら無視する
  if (minDist < THRESHOLD) {
    return neighbor;
  }
  
  // 最近傍点が見つからない場合
  return -1;
}

/**
 * 画像1のキーポイントと近い画像2のキーポイントを組にして返す
 *
 * @param[in]  srcKeyPoints       画像1のキーポイント
 * @param[in]  srcDescriptors     画像1の特徴ベクトル
 * @param[in]  destKeyPoints       画像2のキーポイント
 * @param[in]  destDescriptors     画像2の特徴ベクトル
 * @param[out] ptpairs          類似キーポイントインデックスの列（2つ1組）
 *
 * @return なし
 */
void ObjectDetector::findPairs (CvSeq* srcKeyPoints, CvSeq* srcDescriptors,
               CvSeq* destKeyPoints, CvSeq* destDescriptors,
               vector<CvPoint2D32f>& srcPoints, vector<CvPoint2D32f>& dstPoints) {
  // ptpairs.clear();
  // 画像1の各キーポイントに関して最近傍点を検索
  for (int i = 0; i < srcDescriptors->total; i++) {
    CvSURFPoint* pt1 = (CvSURFPoint*)cvGetSeqElem(srcKeyPoints, i);
    float* desc1 = (float*)cvGetSeqElem(srcDescriptors, i);
    // 最近傍点を検索
    int nn = nearestNeighbor(desc1, pt1->laplacian, destKeyPoints, destDescriptors);
    // 見つかったら画像1のインデックスと画像2のインデックスを順番に登録
    if (nn >= 0) {
      CvSURFPoint* pt2 = (CvSURFPoint*)cvGetSeqElem(destKeyPoints, nn);
      srcPoints.push_back(pt1->pt);
      dstPoints.push_back(pt2->pt);
      //ptpairs.push_back(i);
      //ptpairs.push_back(nn);
    }
  }
}

CvPoint* ObjectDetector::detect (IplImage* destImage) {
    
  CvSeq *destKeyPoints = 0, *destDescriptors = 0;
  CvMemStorage* storage = cvCreateMemStorage(0);
  CvSURFParams params = cvSURFParams(500, 1);
  
  cvExtractSURF(destImage, 0, &destKeyPoints, &destDescriptors, storage, params);
  
  printf("srcNumber: %d\n", srcDescriptors->total);
  printf("destNumber: %d\n", destDescriptors->total);

  // 特徴ベクトルの類似度が高いキーポイント同士を線でつなぐ
  vector<int> ptpairs;
  vector<CvPoint2D32f> srcPoints;
  vector<CvPoint2D32f> dstPoints;
  CvMat srcMat, dstMat;
  double h[9];
  CvMat homography = cvMat(3, 3, CV_64F, h);
  
  findPairs(srcKeyPoints, srcDescriptors, destKeyPoints, destDescriptors, srcPoints, dstPoints);
  int n = srcPoints.size();
  printf("matched points: %d\n", n);
  if (n < (srcKeyPoints->total/15)) return NULL;

  srcMat = cvMat(1, n, CV_32FC2, &srcPoints[0]);
  dstMat = cvMat(1, n, CV_32FC2, &dstPoints[0]);
  
  if (!cvFindHomography(&srcMat, &dstMat, &homography, CV_RANSAC, 5)) {
  //if (!cvFindHomography(&dstMat, &srcMat, &homography, CV_RANSAC, 5)) {
    printf("error findHomography\n");
    return NULL;
  }
  
  // print homograpy
  for (int i=0; i<3; i++) {
    for (int j=0; j<3; j++) {
      printf("%f,", CV_MAT_ELEM(homography, double, i, j));
    }
    printf("\n");
  }
  // 4隅のホモグラフィ変換後の座標を求める
  CvPoint *cornerPts2 = (CvPoint *) cvAlloc (sizeof (CvPoint) * 4);
  CvPoint *pts = (CvPoint *) cvAlloc (sizeof (CvPoint) * 4);
  CvMat *src = cvCreateMat(3, 1, CV_64F);
  CvMat *dst = cvCreateMat(3, 1, CV_64F);
  double xs[4] = {0, srcWidth, srcWidth, 0};
  double ys[4] = {0, 0, srcHeight, srcHeight};
  CvSeq* res;
  res = cvCreateSeq (CV_SEQ_ELTYPE_POINT , sizeof (CvSeq), sizeof (CvPoint), storage);
  for (int i=0; i<4; i++) {
    printf("warp (%f,%f)\n", xs[i], ys[i]);
    cornerPts2[i].x = xs[i];
    cornerPts2[i].y = ys[i];
    
    cvmSet(src, 0, 0, xs[i]);
    cvmSet(src, 1, 0, ys[i]);
    cvmSet(src, 2, 0, 1.0);
    cvMatMul(&homography, src, dst);
    double x = CV_MAT_ELEM(*dst, double, 0, 0);
    double y = CV_MAT_ELEM(*dst, double, 1, 0);
    double z = CV_MAT_ELEM(*dst, double, 2, 0);
    // 同次座標から本来の座標を求める
    //CvPoint p = cvPoint((int)(x/z), (int)(y/z));
    //printf(" => (%d,%d)\n", p.x, p.y);
    //cvSeqPush(res, &p);
    pts[i].x = (int)(x/z);
    pts[i].y = (int)(y/z);
    printf(" => (%d,%d)\n", pts[i].x, pts[i].y);
  }

  // 後始末
  cvClearSeq(destKeyPoints);
  cvClearSeq(destDescriptors);
  cvReleaseMemStorage(&storage);
  
  return pts;
}
