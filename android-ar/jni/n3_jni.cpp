#include <n3_jni.h>
#include <Tracker.h>
#include <opencv2/core/core.hpp>
#include <opencv2/contrib/detection_based_tracker.hpp>

#include <string>
#include <vector>

using namespace std;
using namespace cv;
using namespace Apps;

inline void vector_Rect_to_Mat(vector<Rect>& v_rect, Mat& mat) {
	mat = Mat(v_rect, true);
}

JNIEXPORT jlong JNICALL Java_com_polysfactory_n3_jni_N3_nativeCreateObject(
		JNIEnv * jenv, jclass, jstring jRedMarker, jstring jBlueMarker) {
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeCreateObject enter");
	const char* redMarker = jenv->GetStringUTFChars(jRedMarker, NULL);
	const char* blueMarker = jenv->GetStringUTFChars(jBlueMarker, NULL);
	jlong tracker;
	try {
		tracker = (jlong) new Tracker(string(redMarker), string(blueMarker));
	} catch (cv::Exception& e) {
		LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeCreateObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
		return 0;
	}

	LOGD("Java_com_polysfactory_n3_jni_N3_nativeCreateObject exit");
	return tracker;
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_N3_nativeDestroyObject(
		JNIEnv * jenv, jclass, jlong thiz) {
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeDestroyObject enter");
	try {
	} catch (cv::Exception& e) {
		LOGD("nativeestroyObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeDestroyObject caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeDestroyObject exit");
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_N3_nativeStart(
		JNIEnv * jenv, jclass, jlong thiz) {
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeStart enter");
	try {
	} catch (cv::Exception& e) {
		LOGD("nativeStart caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeStart caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeStart exit");
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_N3_nativeStop(
		JNIEnv * jenv, jclass, jlong thiz) {
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeStop enter");
	try {
	} catch (cv::Exception& e) {
		LOGD("nativeStop caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeStop caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeStop exit");
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_N3_nativeProcess(
		JNIEnv * jenv, jclass, jlong thiz, jlong imageRgba) {
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeDetect enter");
	try {
		((Tracker*) thiz)->process(*((Mat*) imageRgba), *((Mat*) imageRgba));
	} catch (cv::Exception& e) {
		LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeDetect caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeDetect exit");
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_N3_nativeFindMarkers(
		JNIEnv * jenv, jclass, jlong thiz, jlong imageRgba, jlong rRect,
		jlong bRect) {
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeFindMarkers enter");
	try {
		//converters.h
		//void Mat_to_vector_Rect(cv::Mat& mat, std::vector<cv::Rect>& v_rect);
		//void vector_Rect_to_Mat(std::vector<cv::Rect>& v_rect, cv::Mat& mat);
		Mat& rRectMat = *((Mat*) rRect);
		Mat& bRectMat = *((Mat*) bRect);
		vector<Rect> rRectVector;
		vector<Rect> bRectVector;
		((Tracker*) thiz)->findMarkers(*((Mat*) imageRgba), rRectVector, bRectVector);
		vector_Rect_to_Mat(rRectVector, rRectMat);
		vector_Rect_to_Mat(bRectVector, bRectMat);
	} catch (cv::Exception& e) {
		LOGD("nativeFindMarkers caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeFindMarkers caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
	LOGD("Java_com_polysfactory_n3_jni_N3_nativeFindMarkers exit");
}

JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_N3_nativeSetSize(
		JNIEnv * jenv, jclass, jlong thiz, jint srcWidth, jint srcHeight,
		jint destWidth, jint destHeight) {
	try {
		((Tracker*) thiz)->setSize((int) srcWidth, (int) srcHeight,
				(int) destWidth, (int) destHeight);
	} catch (cv::Exception& e) {
		LOGD("nativeCreateObject caught cv::Exception: %s", e.what());
		jclass je = jenv->FindClass("org/opencv/core/CvException");
		if (!je)
			je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je, e.what());
	} catch (...) {
		LOGD("nativeDetect caught unknown exception");
		jclass je = jenv->FindClass("java/lang/Exception");
		jenv->ThrowNew(je,
				"Unknown exception in JNI code {highgui::VideoCapture_n_1VideoCapture__()}");
	}
}
