#include "GestureLight.h"
#include <cmath>

#define KEEP_MAX 10
#define KEEP_COUNT 15
#define NEAR_THRESHOLD 50
#define DONT_MOVE_THRESHOLD_RATE 0.8
#define SCORE_THRESHOLD 0.85
#define INTERVAL_SEC 3
#define WORLD_SCALE 10.0F

inline Point2D toPoint2D (cv::Point p) {
  return Point2D(p.x, p.y);
}

inline cv::Point toCvPoint (Point2D p) {
    return cv::Point(p.x, p.y);
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

    GestureLight::GestureLight () {
        g.loadTemplates();
        
        /*
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
         */
    }
  
    void GestureLight::trackMarker (cv::Mat& destImg, cv::Point _r, cv::Point _b, cv::Point _g, cv::Point _y) {
        // draw
        /*
        world->Step(1.0F/6.0F, 10, 10);
        line(destImg, cv::Point(0,HEIGHT), cv::Point(1000,HEIGHT), cv::Scalar(0,255,0), 3);
        for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
            //LOGD("**draw body\n");
            Box2DData* userData = (Box2DData*)b->GetUserData();
            if (userData != NULL) {
                if (strcmp(userData->type, "Circle") == 0) {
                    //b2Vec2 v = b->GetWorldCenter();
                    b2Vec2 v = b->GetPosition();
                    //LOGD("** x=%f y=%f r=%f\n", v.x, v.y, userData->radius);
                    cv::Point center = cv::Point(v.x*WORLD_SCALE, v.y*WORLD_SCALE);
                    circle(destImg, center, userData->radius*WORLD_SCALE, cv::Scalar(255,0,0), -1);
                } else if (strcmp(userData->type, "Box") == 0) {
                    world->DestroyBody(b);
                }
            }
        }
         */

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
         //LOGD("dist=%f\n", d);
         if (d < NEAR_THRESHOLD) ++nearCount;
         }
         if (nearCount > (double)actual * DONT_MOVE_THRESHOLD_RATE) {
         // marker is not moving, so clear the path
         LOGD("cleared\n");
         path.clear();
         }
         */
        
        path.push_back(r);

        // decide if we should recognize
        time_t current;
        time(&current);
        double interval = difftime(current, lastTime);
        LOGD("interval=%f\n", interval);
        if (interval < INTERVAL_SEC) return;

        len = path.size();
        if (len < 5) return;

        RecognitionResult res = g.recognize(path);
        LOGD("%s:%f\n", res.name.c_str(), res.score);
        if (res.name == "Circle" && res.score > SCORE_THRESHOLD) {
            LOGD("##circle detect##\n");
            /*
            // convert to vector<Point2D> to CvSeq<CvPoint>
            vector<cv::Point> points;
            for (int i=0; i<len; i++) {
                cv::Point pt = toCvPoint(path[i]);
                points.push_back(pt);
            }
            cv::RotatedRect ellipse = cv::fitEllipse(points);
            float radius = std::min(ellipse.size.width, ellipse.size.height)/(4.0F*WORLD_SCALE);
            cv::ellipse(destImg, ellipse, CV_RGB(0,255,255), -1);

            // add Box2D object
            {
                b2CircleShape cs;
                cs.m_radius = radius;
                LOGD(" x=%f y=%f radius:%f\n", ellipse.center.x/WORLD_SCALE, ellipse.center.y/WORLD_SCALE, radius);
                b2BodyDef bd;
                bd.type = b2_dynamicBody;
                bd.position.Set(ellipse.center.x/WORLD_SCALE, ellipse.center.y/WORLD_SCALE);
                Box2DData* obj = new Box2DData();
                strcpy(obj->type, "Circle");
                obj->radius = radius;
                bd.userData = obj;
                b2Body* body1 = world->CreateBody(&bd);
                b2FixtureDef fixtureDef;
                fixtureDef.shape = &cs;
                fixtureDef.density = 1.0f;
                fixtureDef.friction = 0.3f;
                fixtureDef.restitution = 0.6f;
                body1->CreateFixture(&fixtureDef);
            }
             */

            time(&lastTime);

            //cvEllipseBox(destImg, ellipse, CV_RGB(125,125,255));
        }
    }
}