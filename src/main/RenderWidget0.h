#ifndef RenderWidget0_H
#define RenderWidget0_H

#include <QWidget>
#include "GLRenderWidget.h"
#include "SceneManager.h"
#include "Object.h"
#include "TransformGroup.h"
#include "Shape3D.h"

using namespace RE330;

#include <string.h>
using namespace std;

class RenderWidget0 : public GLRenderWidget
{

public:
    RenderWidget0();
    ~RenderWidget0();

    void startAnimation();
    void stopAnimation();

protected:
    // Event handlers. These are virtual methods of the base class.
    // They are called automatically to handle specific events.

    // Called when the render window is ready.
    void initSceneEvent();

    // Called when the scene in the render window needs to be re-rendered.
    void renderSceneEvent();

    // Called when the render widget is resized.
    void resizeRenderWidgetEvent(const QSize &s);

    // Virtual methods of QWidget class. This is a subset. Re-implement any
    // that you would like to handle.
    void timerEvent(QTimerEvent *t);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void toDecimal(int num_colors, float color_list[][3]);
    void setupCamera();
    void setupObjects();
    void setupLights();
	void setupRobot();
    void setupBunnyParty();

private:
    SceneManager *sceneManager;
    Camera *camera;

    // allows more managed storage of objects
    std::map <string, Object(*)> objects;

    // allows easy updating of all objects in scene
    std::map <string, Object(*)>::iterator it;

    Object *object;
    int timerId;

    // For the virtual trackball
    bool tracking;
    QPoint track_start;

    // Use a composite sphere/hyperbolic sheet for the trackball
    // as suggested by http://www.opengl.org/wiki/Trackball
    static const bool USE_COMPOSITE = true;

    bool sceneCreated;

    string HOUSE;
	TransformGroup * bunnyParty;
	//robot
	TransformGroup * robot;
	TransformGroup * headGroup;
	TransformGroup * eyesGroup;
	TransformGroup * leftArm;
	TransformGroup * leftLowerArm;
	TransformGroup * rightArm;
	TransformGroup * rightLowerArm;
	TransformGroup * leftLeg;
	TransformGroup * leftLowerLeg;
	TransformGroup * rightLeg;
	TransformGroup * rightLowerLeg;
	
	Shape3D * torso;
	Shape3D * head;
	Shape3D * leftEye;
	Shape3D * rightEye;
	Shape3D * leftShoulder;
	Shape3D * leftBicep;
	Shape3D * leftElbow;
	Shape3D * leftForearm;
	Shape3D * leftHand;
	Shape3D * rightShoulder;
	Shape3D * rightBicep;
	Shape3D * rightElbow;
	Shape3D * rightForearm;
	Shape3D * rightHand;
	Shape3D * leftHip;
	Shape3D * leftQuad;
	Shape3D * leftKnee;
	Shape3D * leftShin;
	Shape3D * leftFoot;
	Shape3D * rightHip;
	Shape3D * rightQuad;
	Shape3D * rightKnee;
	Shape3D * rightShin;
	Shape3D * rightFoot;
};

#endif // RenderWidget0_H
