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


RenderWidget0::RenderWidget0()
{
    RenderContext *rs = new GLRenderContext();
    sceneManager = 0;
    tracking = false;
    sceneCreated = false;
    camera = false;
    HOUSE = "house";
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
    //setupObjects();
    //create and position lights in scene
    //setupLights();
    //create and position robot in scene
    setupRobot();


    // Trigger timer event every 5ms.
    timerId = startTimer(5);
}

void RenderWidget0::toDecimal(int num_colors, float color_list[][3])
{
    for (int i = 0; i < num_colors; i++) {
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
            Vector4(0,0,5,1),
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

void RenderWidget0::setupRobot()
{
    //robot dimensions
    float const TORSO_HEIGHT = 3;
    float const TORSO_WIDTH = 2;
    float const TORSO_DEPTH = 0.8;
    float const HEAD_DIAMETER = 1;

    float robotColors[1][3];
    robotColors[0][0] = 0.5;
    robotColors[0][1] = 0.5;
    robotColors[0][2] = 0.5;
    //robot
    TransformGroup *robot = new TransformGroup(Matrix4::IDENTITY);
    sceneManager->setRootNode(robot);

    //torso
    objects["torso"] = Shapes::createBox(sceneManager,
                                         TORSO_HEIGHT, TORSO_WIDTH, TORSO_DEPTH,
                                         1,robotColors, false);
    Shape3D *torso = new Shape3D(objects["torso"]);

    robot->addChildNode(torso);

    //head group

    TransformGroup *headGroup = new TransformGroup(Matrix4::IDENTITY);

    //head
    objects["head"] = Shapes::createBox(sceneManager,
                                        HEAD_DIAMETER, HEAD_DIAMETER, TORSO_DEPTH,
                                        1,robotColors, false);
//Shapes::createSphere(sceneManager, HEAD_DIAMETER, 1, 5, 1,
                      //                     robotColors, false);
    Shape3D *head = new Shape3D(objects["head"]);
    objects["head"]->setTransformation(Matrix4::translate(0,1.5,0));

    headGroup->addChildNode(head);

    robot->addChildNode(headGroup);


    /*
    //eyes
    TransformGroup eyesGroup = new TransformGroup(Matrix4::translate(0,0,-0.5));
    objects["leftEye"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                              robotColors, false);
    objects["leftEye"]->setTransformation(Matrix4::translate(-0.2,0,0));
    objects["rightEye"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                               robotColors, false);
    objects["leftEye"]->setTransformation(Matrix4::translate(0.2,0,0));
    Shape3D leftEye = new Shape3D(objects["leftEye"]);
    Shape3D rightEye = new Shape3D(objects["rightEye"]);
    eyesGroup->addChildNode(leftEye);
    eyesGroup->addChildNode(rightEye);

    headGroup->addChildNode(eyesGroup);

    robot->addChildNode(headGroup);

    //left arm
    TransformGroup leftArm = new TransformGroup(Matrix4::translate(-TORSO_WIDTH/2,
                                                                   TORSO_HEIGHT/2,
                                                                   0));
    objects["leftShoulder"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                                   robotColors, false);
    Shape3D leftShoulder = new Shape3D(objects["leftShoulder"]);

    objects["leftBicep"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                              robotColors, false);
    Shape3D leftBicep = new Shape3D(objects["leftBicep"]);
    leftArm->addChildNode(leftShoulder);
    leftArm->addChildNode(leftBicep);

    TransformGroup leftLowerArm = new TransformGroup();
    objects["leftElbow"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                               robotColors, false);
    Shape3D leftElbow = new Shape3D(objects["leftElbow"]);
    objects["leftForearm"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                               robotColors, false);
    Shape3D leftForearm = new Shape3D(objects["leftForearm"]);
    objects["leftHand"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                               robotColors, false);
    Shape 3D leftHand = new Shape3D(objects["leftHand"]);

    leftLowerArm->addChildNode(leftElbow);
    leftLowerArm->addChildNode(leftForearm);
    leftLowerArm->addChildNode(leftHand);

    leftArm->addChildNode(leftLowerArm);
    robot->addChildNode(leftArm);

    //right arm
    TransformGroup rightArm = new TransformGroup(Matrix4::translate(TORSO_WIDTH/2,
                                                                    TORSO_HEIGHT/2,
                                                                    0)););
    objects["rightShoulder"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                                   robotColors, false);
    Shape3D rightShoulder = new Shape3D(objects["rightShoulder"]);

    objects["rightBicep"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                             robotColors, false);
    Shape3D rightBicep = new Shape3D(objects["rightBicep"]);
    rightArm->addChildNode(rightShoulder);
    rightArm->addChildNode(rightBicep);

    TransformGroup rightLowerArm = new TransformGroup();
    objects["rightElbow"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                                robotColors, false);
    Shape3D rightElbow = new Shape3D(objects["rightElbow"]);
    objects["rightForearm"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                               robotColors, false);
    Shape3D rightForearm = new Shape3D(objects["rightForearm"]);
    objects["rightHand"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                               robotColors, false);
    Shape 3D rightHand = new Shape3D(objects["rightHand"]);

    rightLowerArm->addChildNode(rightElbow);
    rightLowerArm->addChildNode(rightForearm);
    rightLowerArm->addChildNode(rightHand);

    rightArm->addChildNode(rightLowerArm);
    robot->addChildNode(rightArm);

    //left leg
    TransformGroup leftLeg = new TransformGroup(Matrix4::translate(-TORSO_WIDTH/2,
                                                                   -TORSO_HEIGHT/2,
                                                                   0)););
    objects["leftHip"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                                   robotColors, false);
    Shape3D leftHip = new Shape3D(objects["leftHip"]);

    objects["leftQuad"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                             robotColors, false);
    Shape3D leftQuad = new Shape3D(objects["leftQuad"]);
    leftLeg->addChildNode(leftHip);
    leftLeg->addChildNode(leftQuad);

    TransformGroup leftLowerLeg = new TransformGroup();
    objects["leftKnee"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                                robotColors, false);
    Shape3D leftKnee = new Shape3D(objects["leftKnee"]);
    objects["leftShin"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                               robotColors, false);
    Shape3D leftShin = new Shape3D(objects["leftShin"]);
    objects["leftFoot"] = Shapes::createBox(sceneManager, .2, .2, 4, 1,
                                               robotColors, false);
    Shape 3D leftFoot = new Shape3D(objects["leftFoot"]);

    leftLowerLeg->addChildNode(leftKnee);
    leftLowerLeg->addChildNode(leftShin);
    leftLowerLeg->addChildNode(leftFoot);

    leftLeg->addChildNode(leftLowerLeg);
    robot->addChildNode(leftLeg);

    //right leg
    TransformGroup rightLeg = new TransformGroup(Matrix4::translate(TORSO_WIDTH/2,
                                                                    -TORSO_HEIGHT/2,
                                                                    0)););
    objects["rightHip"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                              robotColors, false);
    Shape3D rightHip = new Shape3D(objects["rightHip"]);

    objects["rightQuad"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                            robotColors, false);
    Shape3D rightQuad = new Shape3D(objects["rightQuad"]);
    rightLeg->addChildNode(rightHip);
    rightLeg->addChildNode(rightQuad);

    TransformGroup rightLowerLeg = new TransformGroup();
    objects["rightKnee"] = Shapes::createSphere(sceneManager, .2, .2, 4, 1,
                                               robotColors, false);
    Shape3D rightKnee = new Shape3D(objects["rightKnee"]);
    objects["leftShin"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                            robotColors, false);
    Shape3D rightShin = new Shape3D(objects["rightShin"]);
    objects["rightFoot"] = Shapes::createBox(sceneManager, .2, .2, 4, 1,
                                            robotColors, false);
    Shape 3D rightFoot = new Shape3D(objects["rightFoot"]);

    rightLowerLeg->addChildNode(rightKnee);
    rightLowerLeg->addChildNode(rightShin);
    rightLowerLeg->addChildNode(rightFoot);

    rightLeg->addChildNode(rightLowerLeg);
    robot->addChildNode(rightLeg);
    */
}

void RenderWidget0::setupObjects()
{
    // objects[HOUSE] = Shapes::createQuadHouses(sceneManager);
    //objects["sheet"] = Shapes::createSheet(sceneManager);
    //objects[HOUSE] = Shapes::createHouse(sceneManager);

    //textured objects: box and sphere
    //box
    float boxColors[1][3];
    boxColors[1][0] = 0.5;
    boxColors[1][1] = 0.5;
    boxColors[1][2] = 0.5;
    objects["box"] = Shapes::createBox(sceneManager, 1, 1, 1, 1,
                                       boxColors, false);
    objects["box"]->setTransformation(Matrix4::translate(0,1.75,0));

    Material *boxMaterial = new Material();
    QImage *boxTexImg = new QImage("Flanel-Pattern05.png", "PNG");
    Texture *boxTexture = new Texture(boxTexImg);
    boxMaterial->setTexture(boxTexture);

    Shader *boxShader = new Shader("src/Shaders/texture2D.vert",
                                    "src/Shaders/texture2D.frag");
    boxMaterial->setShader(boxShader);
    objects["box"]->setMaterial(*boxMaterial);

    //sheet
    objects["sheet"] = Shapes::createSheet(sceneManager);

    Material *sheetMaterial = new Material();
    QImage *sheetTexImg = new QImage("wood_pole_texture.png", "PNG");
    Texture *sheetTexture = new Texture(sheetTexImg);
    sheetMaterial->setTexture(sheetTexture);

    Shader *sheetShader = new Shader("src/Shaders/texture2D.vert",
                                     "src/Shaders/texture2D.frag");
    sheetMaterial->setShader(sheetShader);

    objects["sheet"]->setMaterial(*sheetMaterial);

    //shading objects: coffeepot and teapot
    //coffeepot
    objects["coffeepot"] = Shapes::readObject(sceneManager, "teapot.obj");
    objects["coffeepot"]->setTransformation(Matrix4::translate(1.5,0,0));

    Material *coffeepotMaterial = new Material();
    coffeepotMaterial->setDiffuse(Vector3(1., 1., 1.));
    coffeepotMaterial->setSpecular(Vector3(1., 1., 1.));
    coffeepotMaterial->setAmbient(Vector3(1., 1., 1.));
    coffeepotMaterial->setShininess(32.0);
    objects["coffeepot"]->setMaterial(*coffeepotMaterial);

    Shader *coffeeShader = new Shader("src/Shaders/diffuse_shading.vert",
                                      "src/Shaders/diffuse_shading.frag");
    coffeepotMaterial->setShader(coffeeShader);


    //teapot
    objects["teapot"] = Shapes::readObject(sceneManager, "teapot.obj");
    objects["teapot"]->setTransformation(Matrix4::translate(-1.5,0,0));

    Material *teapotMaterial = new Material();
    teapotMaterial->setDiffuse(Vector3(0.2, 0.2, 0.2));
    teapotMaterial->setSpecular(Vector3(0.5, 0.5, 0.5));
    teapotMaterial->setAmbient(Vector3(0.3, 0.3, 0.3));
    teapotMaterial->setShininess(256.0);
    objects["teapot"]->setMaterial(*teapotMaterial);

    Shader *teaShader = new Shader("src/Shaders/diffuse_shading.vert",
                                   "src/Shaders/diffuse_shading.frag");
    teapotMaterial->setShader(teaShader);
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

void RenderWidget0::renderSceneEvent()
{
    sceneManager->renderScene();
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
    // Matrix4 m(cos(0.01),-sin(0.01),0,0, sin(0.01),cos(0.01),0,0, 0,0,1,0, 0,0,0,1);
        // Matrix4 m2(1,0,0,0, 0,cos(0.01), -sin(0.01),0, 0,sin(0.01),cos(0.01),0, 0,0,0,1);
        // objects[HOUSE]->setTransformation(m2*m*objects[HOUSE]->getTransformation());
    updateScene();
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

            // rotates all objects in scene via iteration
            for (it = objects.begin(); it != objects.end(); it++ )
            {
                // dereferencing 'it' returns pair from objects map
                // second value of pair is ptr to object
                (*it).second->setTransformation(trackRotation *
                                                (*it).second->getTransformation());
            }
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
