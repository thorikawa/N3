ifeq ($(strip $(OPENCV_HOME)),)
#default OPENCV_HOME
OPENCV_HOME=/usr/local/opencv-2.4.1
endif

N3: Main.o GeometricRecognizer.o Gesture1.o Gunman.o Draw.o PaperDraw.o Tracker.o ObjectDetector.o
	g++ -L$(OPENCV_HOME)/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_legacy -lopencv_nonfree -lopencv_calib3d -lBox2D Main.o GeometricRecognizer.o Gesture1.o Gunman.o Draw.o ObjectDetector.o PaperDraw.o Tracker.o -o N3 
GeometricRecognizer.o: DollarRecognizer/GeometricRecognizer.cpp
	g++ -c DollarRecognizer/GeometricRecognizer.cpp
Gesture1.o: Apps/Gesture1.cpp
	g++ -c -I$(OPENCV_HOME)/include -IBox2D Apps/Gesture1.cpp
Gunman.o: Apps/Gunman.cpp
	g++ -c -I$(OPENCV_HOME)/include -IBox2D Apps/Gunman.cpp
Draw.o: Apps/Draw.cpp
	g++ -c -I$(OPENCV_HOME)/include Apps/Draw.cpp
PaperDraw.o: Apps/PaperDraw.cpp
	g++ -c -I$(OPENCV_HOME)/include Apps/PaperDraw.cpp
Tracker.o: Apps/Tracker.cpp
	g++ -c -I$(OPENCV_HOME)/include -IBox2D Apps/Tracker.cpp
ObjectRecognizer.o: ObjectRecognizer.cpp
	g++ -c -I$(OPENCV_HOME)/include ObjectRecognizer.cpp
ObjectDetector.o: ObjectDetector.cpp
	g++ -c -I$(OPENCV_HOME)/include ObjectDetector.cpp
Main.o: Main.cpp N3.h
	g++ -c -I$(OPENCV_HOME)/include -IBox2D Main.cpp

DumpFeature: DumpFeature.cpp
	g++ -L$(OPENCV_HOME)/lib -I$(OPENCV_HOME)/include -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_legacy -lopencv_nonfree -lopencv_imgproc DumpFeature.cpp -o DumpFeature
	
TestBox2D: TestBox2D.cpp
	g++ -L$(OPENCV_HOME)/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lBox2D -I$(OPENCV_HOME)/include -IBox2D TestBox2D.cpp -o TestBox2D

TestGlut: TestGlut.cpp
	#g++ -LBox2D/Build/freeglut -I/usr/X11/include -L/usr/X11/lib -lfreeglut_static -lGL -lGLU -lGLUT -lX11 TestGlut.cpp -o TestGlut
	g++  -I/System/Library/Frameworks/GLUT.framework/Headers -framework GLUT -framework OpenGL -lobjc TestGlut.cpp -o TestGlut

Dummy3D: Dummy3D.cpp
	g++ -I/System/Library/Frameworks/GLUT.framework/Headers -I$(OPENCV_HOME)/include -L$(OPENCV_HOME)/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_legacy -lopencv_objdetect -framework GLUT -framework OpenGL -lobjc Dummy3D.cpp -o Dummy3D

clean:
	rm *.o N3
