#include <stdio.h>
#include <stdlib.h>
#include <glut.h>
#include <opencv2/opencv.hpp>
#include <math.h>

#define DEBUG 0
#define SCALE 2.0

// light source
GLfloat lightpos[] = { -1.0, 3.0, 5.0, 0.0 };
GLfloat lightcolor[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat teapotColor[] = { 0.6, 0.8, 0.4, 1.0 };
GLfloat specularcolor[] = { 0.3, 0.3, 0.3, 1.0 };
GLfloat shininess[] = { 80 };

// OpenCV
CvCapture* capture;
CvMemStorage* storage;
CvHaarClassifierCascade* cascade;
double rx,ry;

void draw(void) {
  int i, j, k;
  glPushMatrix();
  // ティーポットの反射特性の設定
  glMaterialfv(GL_FRONT_AND_BACK,
               GL_SPECULAR, specularcolor);
  glMaterialfv(GL_FRONT_AND_BACK,
               GL_SHININESS, shininess);
  glMaterialfv(GL_FRONT_AND_BACK,
               GL_DIFFUSE, teapotColor);
  //glTranslated(0.0, 0.7, 0.0);
  //glRotated(-30, 0.0, 0.0, 1.0);
  //glRotatef(angle, 0.0, 1.0, 0.0);
  glutSolidTeapot(0.5);
  glPopMatrix();
}

void display(void) {
  // check face position
  IplImage* img = cvQueryFrame(capture);
  if (img) {
    printf("img=(%d,%d)\n", img->width, img->height);
    cvClearMemStorage(storage);
    
    IplImage* temp = cvCreateImage(cvSize(img->width/SCALE,img->height/SCALE), 8, 3);
    cvResize(img, temp);

    CvSeq* faces = cvHaarDetectObjects(temp, cascade, storage,
                                       1.3, 2,
                                       //CV_HAAR_DO_CANNY_PRUNING,
                                       CV_HAAR_FIND_BIGGEST_OBJECT,
                                       cvSize(100, 100));
    //printf("%d faces are found\n", faces->total);

    if (faces->total > 0) {
      CvRect* r = (CvRect*)cvGetSeqElem(faces, 0);
      CvPoint pt1 = cvPoint(r->x, r->y);
      CvPoint pt2 = cvPoint(r->x + r->width, r->y + r->height);
      int x = r->x + (r->width/2);
      int y = r->y + (r->height/2);
      double hw = (double)(temp->width/2);
      double hh = (double)(temp->height/2);
      rx = (x-hw)/hw;
      ry = (y-hh)/hh;
      rx *= 1.5;
      ry *= 1.5;
      printf("eye=(%f,%f)\n", rx, ry);
      cvRectangle( temp, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
    } else {
      printf("no face\n", rx, ry);
    }
#if DEBUG
    cvShowImage("My Window", temp);
#endif
  }

  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(50, 1.0, 1.0, 100.0);
  gluLookAt(
            //1.2+cos(rx), 1.0+sin(rx), 2.0+ry,
            0.0-rx, 1.0-ry, 2.5,
            //-1.0, -1.0, -2.0,
            0.0, 0.0, 0.0,
            0.0, 1.0, 0.0);
  // model view
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // Light source
  glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
  
  // draw teapot
  draw();

  glutSwapBuffers();
}

void idle(void) {
  glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y) {
  if (key == 'f') {
    glutEnterGameMode();
  } else if (key == 'g') {
    printf("leave game mode\n");
    glutLeaveGameMode();
  } else if (key == 27) {
    exit(-1);
  }
}

int main(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutCreateWindow("title");
  glutInitWindowSize(800,600);
  glutInitWindowPosition(100,100);
#if DEBUG
#else
  glutFullScreen();
#endif
  glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE
                      | GLUT_DEPTH);

  //glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutIdleFunc(idle);
  glutKeyboardFunc(keyboard);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, lightcolor);
  glLightfv(GL_LIGHT0, GL_SPECULAR, lightcolor);
  
  // initialize OpenCV
  capture = cvCaptureFromCAM(CV_CAP_ANY);  
  if (!capture) {
    fprintf(stderr, "ERROR: capture is NULL \n");
    return -1;
  }
  storage = cvCreateMemStorage(0);
  cascade = (CvHaarClassifierCascade*)cvLoad("./data/haarcascade_frontalface_default.xml", 0, 0, 0);
  if (!cascade) {
    fprintf(stderr, "ERROR: cascade is NULL \n");
    return -1;
  }
#if DEBUG
  cvNamedWindow("My Window", 1);
#endif
  rx = .0;
  ry = .0;
  
  glutMainLoop();
  return 0;
}