#include "Gesture1.h"
#include <cmath>

#define KEEP_MAX 30
#define KEEP_COUNT 15
#define NEAR_THRESHOLD 50
#define DONT_MOVE_THRESHOLD_RATE 0.8
#define SCORE_THRESHOLD 0.80
#define INTERVAL_SEC 2
#define WORLD_SCALE 5.0F

inline Point2D toPoint2D (CvPoint p) {
  return Point2D(p.x, p.y);
}

inline CvPoint toCvPoint (Point2D p) {
  return cvPoint(p.x, p.y);
}

inline double dist (Point2D p1, Point2D p2) {
  double dx = p2.x - p1.x;
  double dy = p2.y - p1.y;
  return sqrt((dx * dx) + (dy * dy));  
}


namespace Apps
{
  class Box2DData {
  public:
    char type[32];
    float radius;
  };

  Gesture1::Gesture1 () {
    g.loadTemplates();
    
    // Box2D setup
    b2Vec2 gravity(0.0f, 10.0f);
    bool doSleep = true;
    world = new b2World(gravity, doSleep);

    b2Body* ground = NULL;
    b2BodyDef bd;
    ground = world->CreateBody(&bd);
    
    b2EdgeShape shape;
    shape.Set(b2Vec2(0.0f, (float)HEIGHT/WORLD_SCALE), b2Vec2((float)WIDTH/WORLD_SCALE, (float)HEIGHT/WORLD_SCALE));
    //shape.Set(b2Vec2(0.0f, 150.0f), b2Vec2(1000.0f, 150.0f));
    ground->CreateFixture(&shape, 0.0f);

    IplImage* srcImage = cvLoadImage("kleenex.png", CV_LOAD_IMAGE_GRAYSCALE);
    objectDetector = new ObjectDetector();
    objectDetector->loadSourceImage(srcImage);
  }
  
  void Gesture1::trackMarker (IplImage* destImg, CvPoint _r, CvPoint _b, CvPoint _g, CvPoint _y) {
    
    CvPoint* objPoints = objectDetector->detect(destImg);
    if (objPoints != NULL) {
      cvFillConvexPoly (destImg, objPoints, 4, CV_RGB(0, 255, 255), CV_AA, 0);
      for (int i=0; i<4; i++) {
        cvCircle(destImg, objPoints[i], 3, CV_RGB(0,255,0), -1);
      }
    }

    // draw
    world->Step(1.0F/12.0F, 10, 10);
    cvLine(destImg, cvPoint(0,HEIGHT), cvPoint(1000,HEIGHT), CV_RGB(0,255,0), 3);
    for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
      printf("**draw body\n");
      Box2DData* userData = (Box2DData*)b->GetUserData();
      if (userData != NULL) {
        //b2Vec2 v = b->GetWorldCenter();
        b2Vec2 v = b->GetPosition();
        printf("** x=%f y=%f r=%f\n", v.x, v.y, userData->radius);
        CvPoint center = cvPoint(v.x*WORLD_SCALE, v.y*WORLD_SCALE);
        cvCircle(destImg, center, userData->radius*WORLD_SCALE, CV_RGB(255,0,0), -1);
      }      
    }

    if (_r.x < 0) return;
    Point2D r = toPoint2D(_r);
    
    // if marker is not moving for a while, reset the path
    int len = path.size();
    if (len > KEEP_MAX) {
      path.erase(path.begin());
    }
    int nearCount = 0;
    int actual = min(KEEP_COUNT, len);
    
    /*
     for(int i=0; i<actual; i++){
     Point2D p = path[len-1-i];
     double d = dist(p, r);
     //printf("dist=%f\n", d);
     if (d < NEAR_THRESHOLD) ++nearCount;
     }
     if (nearCount > (double)actual * DONT_MOVE_THRESHOLD_RATE) {
     // marker is not moving, so clear the path
     printf("cleared\n");
     path.clear();
     }
     */
    
    path.push_back(r);

    // decide if we should recognize
    time_t current;
    time(&current);
    double interval = difftime(current, lastTime);
    printf("interval=%f\n", interval);
    if (interval < INTERVAL_SEC) return;

    len = path.size();
    if (len < 5) return;

    RecognitionResult res = g.recognize(path);
    printf("%s:%f\n", res.name.c_str(), res.score);
    if (res.name == "Circle" && res.score > SCORE_THRESHOLD) {
      printf("##circle detect##\n");
      // convert to vector<Point2D> to CvSeq<CvPoint>
      CvSeqWriter writer;
      CvMemStorage* storage = cvCreateMemStorage(0);
      cvStartWriteSeq( CV_32SC2, sizeof(CvSeq), sizeof(CvPoint), storage, &writer);
      for (int i=0; i<len; i++) {
        CvPoint pt = toCvPoint(path[i]);
        CV_WRITE_SEQ_ELEM(pt, writer);
      }
      CvSeq* seq = cvEndWriteSeq(&writer);
      CvBox2D ellipse = cvFitEllipse2(seq);
      float radius = std::min(ellipse.size.width, ellipse.size.height)/(4.0F*WORLD_SCALE);
      cvEllipseBox(destImg, ellipse, CV_RGB(0,255,255), -1);
      //boxes.push_back(ellipse);

      // add Box2D object
      b2CircleShape cs;
      cs.m_radius = radius;
      printf(" x=%f y=%f radius:%f\n", ellipse.center.x/WORLD_SCALE, ellipse.center.y/WORLD_SCALE, radius);
      b2BodyDef bd;
      bd.type = b2_dynamicBody;
      bd.position.Set(ellipse.center.x/WORLD_SCALE, ellipse.center.y/WORLD_SCALE);
      Box2DData* obj = new Box2DData();
      obj->radius = radius;
      bd.userData = obj;
      b2Body* body1 = world->CreateBody(&bd);
      b2FixtureDef fixtureDef;
      fixtureDef.shape = &cs;
      fixtureDef.density = 1.0f;
      fixtureDef.friction = 0.3f;
      fixtureDef.restitution = 0.6f;
      body1->CreateFixture(&fixtureDef);

      time(&lastTime);

      //cvEllipseBox(destImg, ellipse, CV_RGB(125,125,255));
    }
  }
}