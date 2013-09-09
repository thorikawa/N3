ifeq ($(strip $(OPENCV_HOME)),)
#default OPENCV_HOME
OPENCV_HOME=/usr/local/opencv-2.4.1
endif

CXX=g++
CXXFLAGS=-I$(OPENCV_HOME)/include -IBox2D
LDFLAGS=-L${OPENCV_HOME}/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_legacy -lopencv_nonfree -lopencv_calib3d -lBox2D

N3: bin/Main.o bin/DollarRecognizer/GeometricRecognizer.o bin/Apps/Gesture1.o bin/Apps/Gunman.o bin/Apps/Draw.o bin/Apps/PaperDraw.o bin/Apps/Tracker.o bin/ObjectDetector.o
	${CXX} ${LDFLAGS} bin/Main.o bin/DollarRecognizer/GeometricRecognizer.o bin/Apps/Gesture1.o bin/Apps/Gunman.o bin/Apps/Draw.o bin/Apps/PaperDraw.o bin/Apps/Tracker.o bin/ObjectDetector.o -o N3 
bin/%.o : %.cpp
	@mkdir -p $(dir $@)
	${CXX} ${CXXFLAGS} -c $< -o $@

DumpFeature: DumpFeature.cpp
	${CXX} -L$(OPENCV_HOME)/lib -I$(OPENCV_HOME)/include -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_legacy -lopencv_nonfree -lopencv_imgproc DumpFeature.cpp -o DumpFeature
	
TestBox2D: TestBox2D.cpp
	${CXX} -L$(OPENCV_HOME)/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lBox2D -I$(OPENCV_HOME)/include -IBox2D TestBox2D.cpp -o TestBox2D

TestGlut: TestGlut.cpp
	#${CXX} -LBox2D/Build/freeglut -I/usr/X11/include -L/usr/X11/lib -lfreeglut_static -lGL -lGLU -lGLUT -lX11 TestGlut.cpp -o TestGlut
	${CXX}  -I/System/Library/Frameworks/GLUT.framework/Headers -framework GLUT -framework OpenGL -lobjc TestGlut.cpp -o TestGlut

Dummy3D: Dummy3D.cpp
	${CXX} -I/System/Library/Frameworks/GLUT.framework/Headers -I$(OPENCV_HOME)/include -L$(OPENCV_HOME)/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_legacy -lopencv_objdetect -framework GLUT -framework OpenGL -lobjc Dummy3D.cpp -o Dummy3D

clean:
	rm -rf bin N3
