#include <n3_marker_jni.h>
#include <MarkerDetector.hpp>
#include <BGRAVideoFrame.h>
#include <../N3.h>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

#include <string>
#include <vector>

using namespace std;
using namespace cv;

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat) {
	mat = Mat(v_rect, true);
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeFindMarkers
  (JNIEnv * jenv, jobject, jlong thiz, jlong imageBgra, jlong) {
	try {
		MarkerDetector* markerDetector = (MarkerDetector*) thiz;
		Mat* image = (Mat*) imageBgra;
		markerDetector->processFrame(*image);
		std::vector<Transformation> transformations = markerDetector->getTransformations();
	} catch (...) {
		LOGD("nativeDetect caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
}
