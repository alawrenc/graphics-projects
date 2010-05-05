#ifndef RenderWidget0_H
#define RenderWidget0_H

#include <QWidget>
#include "GLRenderWidget.h"
#include "SceneManager.h"
#include "Object.h"
#include "TransformGroup.h"
#include "Shape3D.h"
#include <QTime>

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
    void keyPressEvent ( QKeyEvent * k );
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    void toDecimal(int num_colors, float color_list[][3]);
    void setupCamera();
    void setupLights();
    void setupStillLife();

private:
    SceneManager *sceneManager;
    Camera *camera;
    RenderContext *rs;

    // allows more managed storage of objects
    std::map <string, Object(*)> objects;

    // allows easy updating of all objects in scene
    std::map <string, Object(*)>::iterator it;

    Object *object;
    int timerId;
    int frames;
    float timebase;
    QTime *timer;

    // For the virtual trackball
    bool tracking;
    QPoint track_start;

    // Use a composite sphere/hyperbolic sheet for the trackball
    // as suggested by http://www.opengl.org/wiki/Trackball
    static const bool USE_COMPOSITE = true;

    bool sceneCreated;

    TransformGroup * world;

};
#endif // RenderWidget0_H
