/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_polysfactory_n3_jni_NativeMarkerDetector */

#ifndef _Included_com_polysfactory_n3_jni_NativeMarkerDetector
#define _Included_com_polysfactory_n3_jni_NativeMarkerDetector
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_polysfactory_n3_jni_NativeMarkerDetector
 * Method:    nativeCreateObject
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeCreateObject
  (JNIEnv *, jobject);

/*
 * Class:     com_polysfactory_n3_jni_NativeMarkerDetector
 * Method:    nativeFindMarkers
 * Signature: (JJJ)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeFindMarkers
  (JNIEnv *, jobject, jlong, jlong, jlong);

/*
 * Class:     com_polysfactory_n3_jni_NativeMarkerDetector
 * Method:    nativeDestroyObject
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_jni_NativeMarkerDetector_nativeDestroyObject
  (JNIEnv *, jobject, jlong);

#ifdef __cplusplus
}
#endif
#endif