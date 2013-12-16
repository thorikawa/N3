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

void vector_Transformation_to_Mat(vector<Transformation>& v_tr, Mat& mat) {
	int count = (int) v_tr.size();
	mat.create(count, 16, CV_32FC1);
	for (int i = 0; i < count; i++) {
		Transformation tr = v_tr[i];
		Matrix44 m44 = tr.getMat44();
		for (int j = 0; j < 16; j++) {
			mat.at<float>(i, j) = m44.data[j];
		}
	}
}

#if 0
// reference
void vector_KeyPoint_to_Mat(vector<KeyPoint>& v_kp, Mat& mat)
{
	int count = (int)v_kp.size();
	mat.create(count, 1, CV_32FC(7));
	for(int i=0; i<count; i++)
	{
		KeyPoint kp = v_kp[i];
		mat.at< Vec<float, 7> >(i, 0) = Vec<float, 7>(kp.pt.x, kp.pt.y, kp.size, kp.angle, kp.response, (float)kp.octave, (float)kp.class_id);
	}
}

//vector_Mat
void Mat_to_vector_Mat(cv::Mat& mat, std::vector<cv::Mat>& v_mat)
{
	v_mat.clear();
	if(mat.type() == CV_32SC2 && mat.cols == 1)
	{
		v_mat.reserve(mat.rows);
		for(int i=0; i<mat.rows; i++)
		{
			Vec<int, 2> a = mat.at< Vec<int, 2> >(i, 0);
			long long addr = (((long long)a[0])<<32) | (a[1]&0xffffffff);
			Mat& m = *( (Mat*) addr );
			v_mat.push_back(m);
		}
	} else {
		LOGD("Mat_to_vector_Mat() FAILED: mat.type() == CV_32SC2 && mat.cols == 1");
	}
}

void vector_Mat_to_Mat(std::vector<cv::Mat>& v_mat, cv::Mat& mat)
{
	int count = (int)v_mat.size();
	mat.create(count, 1, CV_32SC2);
	for(int i=0; i<count; i++)
	{
		long long addr = (long long) new Mat(v_mat[i]);
		mat.at< Vec<int, 2> >(i, 0) = Vec<int, 2>(addr>>32, addr&0xffffffff);
	}
}
#endif

JNIEXPORT jlong JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeCreateObject(
		JNIEnv * jenv, jobject) {
	jlong detector;
	try {
		// TODO: this is iPad camera matrix...
		CameraCalibration calib = CameraCalibration(
				6.24860291e+02 * (640. / 352.), 6.24860291e+02 * (480. / 288.),
				640 * 0.5f, 480 * 0.5f);
		detector = (jlong) new MarkerDetector(calib);
	} catch (...) {
		LOGD("nativeCreateObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
		return 0;
	}

	return detector;
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeFindMarkers(
		JNIEnv * jenv, jobject, jlong thiz, jlong imageBgra, jlong transMat) {
	try {
		MarkerDetector* markerDetector = (MarkerDetector*) thiz;
		Mat* image = (Mat*) imageBgra;
		Mat* transMatObj = (Mat*) transMat;
		markerDetector->processFrame(*image);
		std::vector<Transformation> transformations =
				markerDetector->getTransformations();
		vector_Transformation_to_Mat(transformations, *transMatObj);
	} catch (...) {
		LOGD("nativeDetect caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeDestroyObject(
		JNIEnv * jenv, jobject, jlong thiz) {
	try {
	} catch (...) {
		LOGD("nativeDestroyObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
}
