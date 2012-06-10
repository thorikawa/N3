#!/bin/sh

g++ -I/usr/local/opencv-2.3.0/include -L/usr/local/opencv-2.3.0/lib -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc objtracking.cpp && ./a.out

