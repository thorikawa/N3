#!/bin/sh

javah -classpath bin/classes -o jni/n3_jni.h com.polysfactory.n3.jni.N3
javah -classpath bin/classes -o jni/n3_marker_jni.h com.polysfactory.n3.jni.NativeMarkerDetector
