#!/bin/sh

project_path=/workspace/mmdunitytry2
ant clean build-jar
mkdir -p $project_path/Assets/Plugins/Android
cp n3-unity.jar  $project_path/Assets/Plugins/Android/
cp AndroidManifest.xml  $project_path/Assets/Plugins/Android/

#ndk-build
cp libs/armeabi-v7a/lib*  $project_path/Assets/Plugins/Android/

cp -r res $project_path/Assets/Plugins/Android/
