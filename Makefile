OPENCV_HOME=/usr/local/opencv-2.4.1

N3: Main.o GeometricRecognizer.o Gesture1.o
	g++ -L$(OPENCV_HOME)/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lBox2D Main.o GeometricRecognizer.o Gesture1.o -o N3 
GeometricRecognizer.o: DollarRecognizer/GeometricRecognizer.cpp
	g++ -c DollarRecognizer/GeometricRecognizer.cpp
Gesture1.o: Apps/Gesture1.cpp
	g++ -c -I$(OPENCV_HOME)/include -IBox2D Apps/Gesture1.cpp
Main.o: Main.cpp
	g++ -c -I$(OPENCV_HOME)/include -IBox2D Main.cpp