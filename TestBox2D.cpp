//#include "cv.h"
//#include "highgui.h"
#include <Box2D/Box2D.h>
#include <opencv2/opencv.hpp>

#define WIDTH 800
#define HEIGHT 600

/**
 * Main
 */
int main (int argc, char* argv[]) {
    
    // Box2D setup
    
    b2Vec2 gravity(0.0f, 10.0f);
    bool doSleep = true;
    b2World* world = new b2World(gravity, doSleep);
    
    b2Body* ground = NULL;
    {
        b2BodyDef bd;
        //bd.restitution = 0.9f;
        ground = world->CreateBody(&bd);
        //ground->restitution = 0.9f;
    }
    
    //b2EdgeShape shape;
    //shape.Set(b2Vec2(0.0f, 200.0f), b2Vec2(1000.0f, 200.0f));
    //shape.restitution = 0.9f;

    {
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

    b2CircleShape cs;
    cs.m_radius = 40.0f;
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

    IplImage *dst = cvCreateImage( cvSize(WIDTH, HEIGHT), IPL_DEPTH_8U, 3);

    while (1) {
        printf("--loop--\n");
        cvSet (dst, cvScalarAll (255), 0);
        for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
            b2Vec2 v = b->GetWorldCenter();
            //b->ApplyForce(b2Vec2(0.0f, 10000.0f), v);
            printf("**draw body\n");
            char* s = (char*)b->GetUserData();
            if (s != NULL) {
                b2Vec2 v = b->GetWorldCenter();
                printf("**center %f %f\n", v.x, v.y);
                CvPoint center = cvPoint(v.x*3.0f, v.y*3.0f);
                // TODO set radius
                cvCircle(dst, center, 10.0, CV_RGB(255,0,0), -1);
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
