ifeq ($(strip $(OPENCV_HOME)),)
#default OPENCV_HOME
OPENCV_HOME=/usr/local/opencv-2.4.1
endif

CXX=g++
CXXFLAGS=-I$(OPENCV_HOME)/include -IBox2D -g
LDFLAGS=-L${OPENCV_HOME}/lib -LBox2D/Build/Box2D -lopencv_core -lopencv_highgui -lopencv_features2d -lopencv_imgproc -lopencv_legacy -lopencv_nonfree -lopencv_calib3d -lBox2D
BUILD_DIR=bin
SRCS= \
	Main.cpp \
	DollarRecognizer/GeometricRecognizer.cpp \
	Apps/Gesture1.cpp \
	Apps/Gunman.cpp \
	Apps/Draw.cpp \
	Apps/PaperDraw.cpp \
	Apps/Tracker.cpp \
	ObjectDetector.cpp
OBJS=${SRCS:%.cpp=${BUILD_DIR}/%.o}

N3: $(OBJS)
	${CXX} ${LDFLAGS} ${OBJS} -o N3 
${BUILD_DIR}/%.o : %.cpp
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
