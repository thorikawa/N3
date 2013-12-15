/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_polysfactory_n3_unity_N3 */

#ifndef _Included_com_polysfactory_n3_unity_N3
#define _Included_com_polysfactory_n3_unity_N3
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeCreateObject
 * Signature: (Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_polysfactory_n3_unity_N3_nativeCreateObject
  (JNIEnv *, jclass, jstring, jstring);

/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeDestroyObject
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_unity_N3_nativeDestroyObject
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeStart
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_unity_N3_nativeStart
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeStop
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_unity_N3_nativeStop
  (JNIEnv *, jclass, jlong);

/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeProcess
 * Signature: (JJ)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_unity_N3_nativeProcess
  (JNIEnv *, jclass, jlong, jlong);

/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeFindMarkers
 * Signature: (JJJJ)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_unity_N3_nativeFindMarkers
  (JNIEnv *, jclass, jlong, jlong, jlong, jlong);

/*
 * Class:     com_polysfactory_n3_unity_N3
 * Method:    nativeSetSize
 * Signature: (JIIII)V
 */
JNIEXPORT void JNICALL Java_com_polysfactory_n3_unity_N3_nativeSetSize
  (JNIEnv *, jclass, jlong, jint, jint, jint, jint);

#ifdef __cplusplus
}
#endif
#endif
