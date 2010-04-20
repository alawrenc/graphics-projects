#ifndef __Shape3D_h__
#define __Shape3D_h__

#include "Leaf.h"
#include "Matrix4.h"
#include "Camera.h"
#include "RenderContext.h"
#include "Object.h"


namespace RE330
{
    class Shape3D : public Leaf
    {

    public:
        Shape3D();

        void updateMatrix();
        void draw(Matrix4 m, RenderContext rc, Camera c)
            {
                setModelView(m);
                //setMaterial(myMaterial);
                rc.render(object);
}
            }

    private:
        Object * object;

    }
}

#endif
