N3: Main.o GeometricRecognizer.o Gesture1.o
	g++ -L/usr/local/opencv-2.3.0/lib -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc Main.o GeometricRecognizer.o Gesture1.o -o N3 
GeometricRecognizer.o: DollarRecognizer/GeometricRecognizer.cpp
	g++ -c DollarRecognizer/GeometricRecognizer.cpp
Gesture1.o: Apps/Gesture1.cpp
	g++ -c -I/usr/local/opencv-2.3.0/include Apps/Gesture1.cpp
Main.o: Main.cpp
	g++ -c -I/usr/local/opencv-2.3.0/include Main.cpp