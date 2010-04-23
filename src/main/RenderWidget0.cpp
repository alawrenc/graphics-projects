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
    setupLights();
    //create and position robot in scene
    //setupRobot();

    setupBunnyParty();


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

void RenderWidget0::setupBunnyParty()
{
    Object * baseBunny = Shapes::readObject(sceneManager, "bunny.obj");
    bunnyParty = new TransformGroup(Matrix4::translate(-50,0,0));
    sceneManager->setRootNode(bunnyParty);

    //teapot
    for (int i = 0; i <= 100; i++)
    {
        Object * bunnyClone = new Object(*baseBunny);

        bunnyClone->setTransformation(Matrix4::translate(i,0,0));

        Material *bunnyMaterial = new Material();
        bunnyMaterial->setDiffuse(Vector3(0.2, 0.2, 0.2));
        bunnyMaterial->setSpecular(Vector3(0.5, 0.5, 0.5));
        bunnyMaterial->setAmbient(Vector3(0.3, 0.3, 0.3));
        bunnyMaterial->setShininess(256.0);
        bunnyClone->setMaterial(*bunnyMaterial);

        Shader *bunnyShader = new Shader("src/Shaders/diffuse_shading.vert",
                                       "src/Shaders/diffuse_shading.frag");
        bunnyMaterial->setShader(bunnyShader);

        bunnyParty->addChildNode(new Shape3D(bunnyClone));

    }
}

void RenderWidget0::setupRobot()
{
    //robot dimensions
    float const TORSO_HEIGHT = 3;
    float const TORSO_WIDTH = 2;
    float const TORSO_DEPTH = 0.8;
    float const HEAD_DIAMETER = 0.75;
    float const EYE_DIAMETER = 0.1;
    float const EYE_OFFSET = 0.2;
    float const JOINT_SIZE = 0.35;
    float const BICEPS_LENGTH = 1.2;
    float const BICEPS_SIZE = 0.4;
    float const HAND_SIZE = 0.3;
    float const LEG_LENGTH = 1.5;
    float const HIP_SIZE = 0.4;

    float robotColors[1][3];
    robotColors[0][0] = 0;
    robotColors[0][1] = 0;
    robotColors[0][2] = 1;

    float eyeColor[1][3];
    eyeColor[0][0] = 0;
    eyeColor[0][1] = 0;
    eyeColor[0][2] = 0;

    float testColor[1][3];
    testColor[0][0] = 0.5;
    testColor[0][1] = 0.5;
    testColor[0][2] = 0.5;

    //robot
    robot = new TransformGroup(Matrix4::translate(5,0,0));
    sceneManager->setRootNode(robot);

    //torso
    Object *torsoObj = Shapes::createBox(sceneManager,
                                         TORSO_HEIGHT, TORSO_WIDTH, TORSO_DEPTH,
                                         1,robotColors, false);
    torso = new Shape3D(torsoObj);

    robot->addChildNode(torso);

    //head group

    headGroup = new TransformGroup(Matrix4::translate(0,2.0,0));

    //head
    Object *headObj = Shapes::createSphere(sceneManager,
                                           HEAD_DIAMETER, 100, 100,
                                           1, robotColors, false);
    head = new Shape3D(headObj);

    headGroup->addChildNode(head);

    robot->addChildNode(headGroup);

    //eyes
    eyesGroup = new TransformGroup(Matrix4::translate(0,0.25,0.75));
    //left eye from robot prespective
    Object *leftEyeObj = Shapes::createSphere(sceneManager, EYE_DIAMETER, 100, 100,
                                              1, eyeColor, false);
    leftEyeObj->setTransformation(Matrix4::translate(EYE_OFFSET,0,0));
    Object *rightEyeObj = Shapes::createSphere(sceneManager, EYE_DIAMETER, 100, 100,
                                               1, eyeColor, false);
    rightEyeObj->setTransformation(Matrix4::translate(-EYE_OFFSET,0,0));
    leftEye = new Shape3D(leftEyeObj);
    rightEye = new Shape3D(rightEyeObj);
    eyesGroup->addChildNode(leftEye);
    eyesGroup->addChildNode(rightEye);

    headGroup->addChildNode(eyesGroup);

    robot->addChildNode(headGroup);

    //left arm
    leftArm = new TransformGroup(Matrix4::translate(TORSO_WIDTH/2,
                                                                   TORSO_HEIGHT/2,
                                                                   0));
    Object *leftShoulderObj = Shapes::createSphere(sceneManager,
                                                   JOINT_SIZE, 100, 100,
                                                   1, robotColors, false);
    leftShoulder = new Shape3D(leftShoulderObj);

    Object *leftBicepObj = Shapes::createBox(sceneManager,
                                             BICEPS_LENGTH, BICEPS_SIZE, BICEPS_SIZE,
                                             1, robotColors, false);
    leftBicepObj->setTransformation(Matrix4::rotateZ(PI/4)*
                                   Matrix4::translate(0,-BICEPS_LENGTH/2,0));
    leftBicep = new Shape3D(leftBicepObj);

    leftArm->addChildNode(leftShoulder);
    leftArm->addChildNode(leftBicep);

    leftLowerArm = new TransformGroup(Matrix4::translate(BICEPS_LENGTH*cos(PI/4),
                                                                         -BICEPS_LENGTH*sin(PI/4),
                                                                         0));
    Object *leftElbowObj = Shapes::createSphere(sceneManager,
                                                JOINT_SIZE, 100, 100,
                                                1, robotColors, false);
    leftElbow = new Shape3D(leftElbowObj);
    Object *leftForearmObj = Shapes::createBox(sceneManager,
                                               BICEPS_LENGTH, 0.3, 0.3,
                                               1, robotColors, false);
    leftForearmObj->setTransformation(Matrix4::rotateZ(PI*3/4)*
                                      Matrix4::translate(0,-BICEPS_LENGTH/2,0));
    leftForearm = new Shape3D(leftForearmObj);
    Object *leftHandObj = Shapes::createSphere(sceneManager,
                                               HAND_SIZE, 100, 100,
                                               1, robotColors, false);
    leftHandObj->setTransformation(Matrix4::rotateZ(PI*3/4)*
                                   Matrix4::translate(0,-BICEPS_LENGTH,0));
    leftHand = new Shape3D(leftHandObj);

    leftLowerArm->addChildNode(leftElbow);
    leftLowerArm->addChildNode(leftForearm);
    leftLowerArm->addChildNode(leftHand);

    leftArm->addChildNode(leftLowerArm);

    robot->addChildNode(leftArm);

    //right arm
    rightArm = new TransformGroup(Matrix4::translate(-TORSO_WIDTH/2,
                                                                    TORSO_HEIGHT/2,
                                                                    0));
    Object *rightShoulderObj = Shapes::createSphere(sceneManager,
                                                   JOINT_SIZE, 100, 100,
                                                   1, robotColors, false);
    rightShoulder = new Shape3D(rightShoulderObj);

    Object *rightBicepObj = Shapes::createBox(sceneManager,
                                             BICEPS_LENGTH, BICEPS_SIZE, BICEPS_SIZE,
                                             1, robotColors, false);
    rightBicepObj->setTransformation(Matrix4::rotateZ(-PI/4)*
                                    Matrix4::translate(0,-BICEPS_LENGTH/2,0));
    rightBicep = new Shape3D(rightBicepObj);

    rightArm->addChildNode(rightShoulder);
    rightArm->addChildNode(rightBicep);

    rightLowerArm = new TransformGroup(Matrix4::translate(-BICEPS_LENGTH*cos(PI/4),
                                                                          -BICEPS_LENGTH*sin(PI/4),
                                                                          0));
    Object *rightElbowObj = Shapes::createSphere(sceneManager,
                                                JOINT_SIZE, 100, 100,
                                                1, robotColors, false);
    rightElbow = new Shape3D(rightElbowObj);
    Object *rightForearmObj = Shapes::createBox(sceneManager,
                                                BICEPS_LENGTH, 0.3, 0.3,
                                                1, robotColors, false);
    rightForearmObj->setTransformation(Matrix4::rotateZ(-PI*3/4)*
                                       Matrix4::translate(0,-BICEPS_LENGTH/2,0));
    rightForearm = new Shape3D(rightForearmObj);
    Object *rightHandObj = Shapes::createSphere(sceneManager,
                                                HAND_SIZE, 100, 100,
                                                1, robotColors, false);
    rightHandObj->setTransformation(Matrix4::rotateZ(-PI*3/4)*
                                    Matrix4::translate(0,-BICEPS_LENGTH,0));
    rightHand = new Shape3D(rightHandObj);

    rightLowerArm->addChildNode(rightElbow);
    rightLowerArm->addChildNode(rightForearm);
    rightLowerArm->addChildNode(rightHand);

    rightArm->addChildNode(rightLowerArm);

    robot->addChildNode(rightArm);

    //left leg
    leftLeg = new TransformGroup(Matrix4::translate(TORSO_WIDTH/2,
                                                                    -TORSO_HEIGHT/2,
                                                                    0));
    Object *leftHipObj = Shapes::createSphere(sceneManager,
                                              HIP_SIZE, 100, 100,
                                              1, robotColors, false);
    leftHip = new Shape3D(leftHipObj);
    Object *leftQuadObj = Shapes::createBox(sceneManager,
                                            LEG_LENGTH, BICEPS_SIZE, BICEPS_SIZE,
                                            1, robotColors, false);
    leftQuadObj->setTransformation(Matrix4::rotateZ(0.1)*
                                   Matrix4::translate(0,-LEG_LENGTH/2,0));
    leftQuad = new Shape3D(leftQuadObj);

    leftLeg->addChildNode(leftHip);
    leftLeg->addChildNode(leftQuad);

    leftLowerLeg = new TransformGroup(Matrix4::translate(LEG_LENGTH*sin(0.1),
                                                                         -LEG_LENGTH*cos(0.1),
                                                                         0));
    Object *leftKneeObj = Shapes::createSphere(sceneManager,
                                                JOINT_SIZE, 100, 100,
                                                1, robotColors, false);
    leftKnee = new Shape3D(leftKneeObj);
    Object *leftShinObj = Shapes::createBox(sceneManager,
                                            LEG_LENGTH, 0.3, 0.3,
                                            1, robotColors, false);
    leftShinObj->setTransformation(Matrix4::translate(0,-LEG_LENGTH/2,0));
    leftShin = new Shape3D(leftShinObj);
    Object *leftFootObj = Shapes::createSphere(sceneManager,
                                               HAND_SIZE, 100, 100,
                                               1, robotColors, false);
    leftFootObj->setTransformation(Matrix4::translate(0,-LEG_LENGTH,0));
    leftFoot = new Shape3D(leftFootObj);

    leftLowerLeg->addChildNode(leftKnee);
    leftLowerLeg->addChildNode(leftShin);
    leftLowerLeg->addChildNode(leftFoot);

    leftLeg->addChildNode(leftLowerLeg);

    robot->addChildNode(leftLeg);


    //right leg
    rightLeg = new TransformGroup(Matrix4::translate(-TORSO_WIDTH/2,
                                                                     -TORSO_HEIGHT/2,
                                                                     0));
    Object *rightHipObj = Shapes::createSphere(sceneManager,
                                               HIP_SIZE, 100, 100,
                                               1, robotColors, false);
    rightHip = new Shape3D(rightHipObj);
    Object *rightQuadObj = Shapes::createBox(sceneManager,
                                             LEG_LENGTH, BICEPS_SIZE, BICEPS_SIZE,
                                             1, robotColors, false);
    rightQuadObj->setTransformation(Matrix4::rotateZ(-0.1)*
                                    Matrix4::translate(0,-LEG_LENGTH/2,0));
    rightQuad = new Shape3D(rightQuadObj);

    rightLeg->addChildNode(rightHip);
    rightLeg->addChildNode(rightQuad);

    rightLowerLeg = new TransformGroup(Matrix4::translate(-LEG_LENGTH*sin(0.1),
                                                                          -LEG_LENGTH*cos(0.1),
                                                                          0));
    Object *rightKneeObj = Shapes::createSphere(sceneManager,
                                                JOINT_SIZE, 100, 100,
                                                1, robotColors, false);
    rightKnee = new Shape3D(rightKneeObj);
    Object *rightShinObj = Shapes::createBox(sceneManager,
                                             LEG_LENGTH, 0.3, 0.3,
                                             1, robotColors, false);
    rightShinObj->setTransformation(Matrix4::translate(0,-LEG_LENGTH/2,0));
    rightShin = new Shape3D(rightShinObj);
    Object *rightFootObj = Shapes::createSphere(sceneManager,
                                                HAND_SIZE, 100, 100,
                                                1, robotColors, false);
    rightFootObj->setTransformation(Matrix4::translate(0,-LEG_LENGTH,0));
    rightFoot = new Shape3D(rightFootObj);

    rightLowerLeg->addChildNode(rightKnee);
    rightLowerLeg->addChildNode(rightShin);
    rightLowerLeg->addChildNode(rightFoot);

    rightLeg->addChildNode(rightLowerLeg);

    robot->addChildNode(rightLeg);
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
    if (robot)
    {
        Matrix4 m3 = Matrix4::rotateY(-0.01);
        robot->setLocalTransform(m3 * robot->getLocalTransform());
    }
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
