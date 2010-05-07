#include "RenderWidget0.h"
#include "Vector3.h"
#include "Matrix4.h"
#include "Camera.h"
#include "GLWidget.h"
#include "GLRenderContext.h"
#include "VertexData.h"
#include "Shapes.h"
#include "Shader.h"
#include <stdio.h>
#include <iostream>
#include "Light.h"
#include "Material.h"
#include "Texture.h"

#include "Node.h"
#include "NodeGroup.h"
#include "TransformGroup.h"
#include "Leaf.h"
#include "Shape3D.h"
#include <QtOpenGL>

#define PI 3.14159265


RenderWidget0::RenderWidget0()
{
    rs = new GLRenderContext();
    rs->culling = false;
    sceneManager = 0;
    tracking = false;
    sceneCreated = false;
    camera = false;
    frames = 0;
    timer = new QTime::QTime();
    timer->start();
    timebase = 0.0f;

    setFocus();
    grabKeyboard();
    setFocusPolicy(Qt::StrongFocus);
}

RenderWidget0::~RenderWidget0()
{
    if(sceneManager)
    {
        delete sceneManager;
    }
}

void RenderWidget0::initSceneEvent()
{
    sceneManager = new SceneManager();
    //create and position the camera
    setupCamera();

    //create and position and objects in scene
    //create and position lights in scene
    setupLights();
    world = new TransformGroup(Matrix4::translate(0,0,0));

    //create still life
    setupStillLife();

    // Trigger timer event every 5ms.
    timerId = startTimer(50);
}

void RenderWidget0::toDecimal(int num_colors, float color_list[][3])
{
    for (int i = 0; i < num_colors; i++)
    {
        color_list[i][0] = color_list[i][0] / 255;
        color_list[i][1] = color_list[i][1] / 255;
        color_list[i][2] = color_list[i][2] / 255;
    }
}

void RenderWidget0::setupCamera()
{
    const int CAMERA_POSITION = 1;

    // Camera
    camera = sceneManager->createCamera();

    if (CAMERA_POSITION == 1)
    {
        // First camera test setting
        camera->createViewMatrix(
            Vector4(0,0,15,1),
            Vector4(0,0,0,1),
            Vector4(0,1,0,0));
    }
    else if (CAMERA_POSITION == 2)
    {
        // Second camera test setting
        camera->createViewMatrix(
            Vector4(-10,10,40,1),
            Vector4(-5,0,0,1),
            Vector4(0,1,0,0));
    }

    camera->createProjectionMatrix(
        1, 100, 1, 60.0/180.0*M_PI);

    sceneCreated = true;
}

// always called after setupcamera so it can get info from it
void RenderWidget0::setupLights()
{
    //faint, overhead, yellow light
    Light *l1 = sceneManager->createLight();
    l1->setPosition(Vector3(0,-10,0));
    l1->setDirection(Vector3(0,1,0));
    l1->setType(Light::DIRECTIONAL);
    l1->setDiffuseColor(Vector3(.5,.5,0));
    l1->setSpecularColor(Vector3(0,0,0));
    l1->setAmbientColor(Vector3(.1,.1,.1));

    //blue point left from the left
    Light *l2 = sceneManager->createLight();
    l2->setPosition(Vector3(-10,0,0));
    l2->setDirection(Vector3(1,0,0));
    l2->setType(Light::POINT);
    l2->setDiffuseColor(Vector3(0,0,0));
    l2->setSpecularColor(Vector3(0,0,5));

    //green spot from lower right
    Light *l3 = sceneManager->createLight();
    l3->setType(Light::SPOT);
    l3->setPosition(Vector3(5,-10,0));
    l3->setSpotDirection(Vector3(0, 1, 0));
    l3->setSpotExponent(24.0);
    l3->setSpotCutoff(40.0);
    l3->setDiffuseColor(Vector3(0,0,0));
    l3->setSpecularColor(Vector3(0,10,0));
}

void RenderWidget0::setupStillLife()
{
    //
    Vector3 cp1 = Vector3(0, 0, 0);
    Vector3 cp2 = Vector3(-1, 1, 0);
    Vector3 cp3 = Vector3(1, 1, 0);
    Vector3 cp4 = Vector3(5, 5, 0);
    Vector3 controlPoints[] = { cp1, cp2, cp3, cp3 };
    Object * vase = Shapes::createBezierShape(sceneManager,
                                              1, controlPoints, 10, 5);
}

void RenderWidget0::renderSceneEvent()
{
    sceneManager->renderScene();

    frames++;
    float time = timer->elapsed();
    char title[10];

    if (time - timebase > 1000) {
        sprintf(title, "FPS:%4.2f",
                frames*1000.0/(time-timebase));
        timebase = time;
        frames = 0;
        this->parentWidget()->setWindowTitle(title);
    }
}

void RenderWidget0::resizeRenderWidgetEvent(const QSize &s)
{
    // If we have a camera and are resizing the widget, be sure to
    // update the aspect ratio!
    if (camera)
        camera->setAspectRatio((float)s.width()/(float)s.height());
}

void RenderWidget0::timerEvent(QTimerEvent *t)
{
    updateScene();
}

void RenderWidget0::keyPressEvent( QKeyEvent * k )
{
    switch ( k->key() )
    {
    case Qt::Key_C:
        rs->culling = !(rs->culling);
        break;

    }
}

void RenderWidget0::mousePressEvent(QMouseEvent *e)
{
    // If we're pressing the left button, then we should start tracking
    if (e->buttons() & Qt::LeftButton)
    {
        tracking = true;

        // Store the initial mouse position
        track_start = e->pos();
    }

}

void RenderWidget0::mouseMoveEvent(QMouseEvent *e)
{
    if (tracking && e->buttons() & Qt::LeftButton && e->pos() != track_start)
    {
        Vector3 start, stop; // Temporary vectors representing
                             // points on the virtual trackball
        float _x, _y, _z; // Temporary 3-points
        const float diameter = (float)(width() < height() ? width() : height());

        // x coord of the start point on the virtual ball
        _x = ((float)track_start.x()*2.f - (float)width())/diameter;
        // y coord of the start point on the virtual ball
        _y = -(((float)track_start.y()*2.f - (float)height())/diameter);

        if (RenderWidget0::USE_COMPOSITE)
        {
            // Use a composite sphere/hyperbolic sheet for the trackball
            // as suggested by http://www.opengl.org/wiki/Trackball
            if (_x*_x + _y*_y <= 1.f/2.f) {
                _z = sqrt(1 - _x*_x - _y*_y);
            } else {
                _z = 1.f/(2*sqrt(_x*_x + _y*_y));
            }
        } else
        {
            // Make sure we're not clicking outside of the sphere
            // (There should be a better way to handle this)
            if (_x*_x + _y*_y > 1) {
                _z = 0;
            } else {
                // z coord of the start point on the virtual ball
                _z = sqrt(1 - _x*_x - _y*_y);
            }
        }

        // Create the starting point vector
        start = Vector3(_x, _y, _z).normalize();

        // x coord of the stop point on the virtual ball
        _x = ((float)e->pos().x()*2.f - (float)width())/diameter;
        // y coord of the stop point on the virtual ball
        _y = -(((float)e->pos().y()*2.f - (float)height())/diameter);

        if (RenderWidget0::USE_COMPOSITE)
        {
            // Use a composite sphere/hyperbolic sheet for the trackball
            // as suggested by http://www.opengl.org/wiki/Trackball
            if (_x*_x + _y*_y <= 1.f/2.f) {
                _z = sqrt(1 - _x*_x - _y*_y);
            } else {
                _z = 1.f/(2*sqrt(_x*_x + _y*_y));
            }
        } else
        {
            // Make sure we're not clicking outside of the sphere
            // (There should be a better way to handle this)
            if (_x*_x + _y*_y > 1) {
                _z = 0;
            } else {
                // z coord of the stop point on the virtual ball
                _z = sqrt(1 - _x *_x - _y*_y);
            }
        }

        // Create the stopping point vector
        stop = Vector3(_x, _y, _z).normalize();

        // Make sure that the vectors aren't equal (if they are,
        // the cross product doesn't exist!)
        // Also, make sure that the cross product isn't 1, as acos(1) = nan
        if (start != stop &&
            !(1 - (start^stop)) < EPSILON & (1 - (start^stop)) > -EPSILON) {
            // Prep the trackball rotation matrix
            Matrix4 trackRotation = Matrix4::rotateA(start*stop,
                                                     acos(start^stop));
            TransformGroup * root = sceneManager->getRootNode();
            root->setLocalTransform(trackRotation * root->getLocalTransform());

            // Update the (now old) mouse position
            track_start = e->pos();
        }
    } else if (tracking && !(e->buttons() & Qt::LeftButton))
    {
        // If we're tracking but somehow not clicking, then we shouldn't
        // actually still be tracking
        tracking = false;
    }
}

void RenderWidget0::mouseReleaseEvent(QMouseEvent *e)
{
    // If we've released the left mouse button, then we shouldn't be tracking
    if (!(e->buttons() & Qt::LeftButton))
    {
        tracking = false;
    }
}

void RenderWidget0::startAnimation()
{
    if(!timerId)
    {
        timerId = startTimer(5);
    }
}

void RenderWidget0::stopAnimation()
{
    if(timerId)
    {
        killTimer(timerId);
        timerId = 0;
    }
}
