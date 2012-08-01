∑//#include "cv.h"
//#include "highgui.h"
#include <Box2D/Box2D.h>
#include <opencv2/opencv.hpp>

#define WIDTH 800
#define HEIGHT 600

b2World* world;

void recreate () {
  printf("recreate body\n");
  b2BodyDef bd;
  //bd.type = b2_staticBody;
  //bd.position.Set(baseX/WORLD_SCALE, baseY/WORLD_SCALE);
  char* obj =  (char *)malloc(100);
  strcpy(obj, "box");
  bd.userData = obj;
  b2Body* body1 = world->CreateBody(&bd);
  
  b2PolygonShape cs;
  b2Vec2 vertices[4] = {
    b2Vec2(40,180),
    b2Vec2(60,180),
    b2Vec2(60,200),
    b2Vec2(40,200)
  };
  cs.Set(vertices, 4);
  body1->CreateFixture(&cs, 0.0f);
}

/**
 * Main
 */
int main (int argc, char* argv[]) {
  
  // Box2D setup
  
  b2Vec2 gravity(0.0f, 10.0f);
  bool doSleep = true;
  world = new b2World(gravity, doSleep);
  
  b2Body* ground = NULL;
  {
    b2BodyDef bd;
    //bd.restitution = 0.9f;
    char obj[256] = "ground";
    bd.userData = obj;
    ground = world->CreateBody(&bd);
    b2EdgeShape shape;
    shape.Set(b2Vec2(0.0f, 200.0f), b2Vec2(1000.0f, 200.0f));
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.0f;
    //ground->CreateFixture(&fixtureDef);
    ground->CreateFixture(&shape, 0.0f);
  }

  recreate();

  {
    b2CircleShape cs;
    cs.m_radius = 3.333f;
    b2BodyDef bd;
    bd.type = b2_dynamicBody;
    bd.position.Set(50,0);
    char obj[256] = "obj";
    bd.userData = obj;
    b2Body* body1 = world->CreateBody(&bd);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &cs;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    fixtureDef.restitution = 0.6f;
    body1->CreateFixture(&fixtureDef);
  }

  IplImage *dst = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);

  while (1) {
    //printf("--loop--\n");
    cvSet (dst, cvScalarAll (255), 0);
    cvLine(dst, cvPoint(0,600), cvPoint(1000,600), CV_RGB(0,255,0), 3);
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
      b2Vec2 v = b->GetWorldCenter();
      //b->ApplyForce(b2Vec2(0.0f, 10000.0f), v);
      //printf("**draw body\n");
      char* s = (char*)b->GetUserData();
      if (s != NULL) {
        printf("%s\n", s);
        if (strcmp(s, "obj") == 0) {
          b2Vec2 v = b->GetWorldCenter();
          //printf("**center %f %f\n", v.x, v.y);
          CvPoint center = cvPoint(v.x*3.0f, v.y*3.0f);
          // TODO set radius
          cvCircle(dst, center, 10.0, CV_RGB(255,0,0), -1);
        } else if (strcmp(s, "box") == 0) {
          world->DestroyBody(b);
          recreate();
        }
      }
    }
    world->Step(1.0F/24.0F, 10, 10);
		cvShowImage("app", dst);
    
    
    char key=(char)cvWaitKey(1);
    if(key=='\033') break;
  }
    cvReleaseImage(&dst);
	return 0;
}
