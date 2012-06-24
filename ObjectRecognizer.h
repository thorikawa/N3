#ifndef OBJECT_RECOGNIZER_H_
#define OBJECT_RECOGNIZER_H_

#include <opencv2/opencv.hpp>
#include <opencv2/legacy/compat.hpp>
#include <opencv2/legacy/legacy.hpp>
#include <opencv2/nonfree/nonfree.hpp>

#include <fstream>
#include <vector>

using namespace std;

class ObjectRecognizer
{
public:
  
  ObjectRecognizer();
  virtual ~ObjectRecognizer();
  /** 指定された画像の特徴点を検出し、LSH特定で物体認識を行う */
  int detect(IplImage* queryImage);
  /** 指定されたファイル名から特徴ベクトルをLSHメモリハッシュに読み込む */
  bool loadDescription(const char* filename);
  
private:  

  const static double THRESHOLD = 0.3;

  int getObjectId (int index);

  /** 検出する特徴ベクトルの次元数 */
  static const int DIM = 128;
  /** LSHメモリハッシュ */
  CvLSH* lsh;  
  /** 追加された物体の数 */
  int objNumber;
  /** 特徴点累計の最後の添字 */
  int lastIndex;
  /** 各オブジェクトにおける最後の特徴点累計添字 */
  vector<int> lastIndexes;
  /** 各オブジェクトの特徴点数 */
  vector<int> numKeypoints;
  
};

#endif /* OBJECT_RECOGNIZER_H_ */
