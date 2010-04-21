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
        Shape3D(Object * o)
            {
                object = o;
            }

        void draw(Matrix4 m, RenderContext &rc, Camera c)
            {
                rc.setModelViewMatrix(c.getViewMatrix() * m *
                                      object->getTransformation());
                //object->setMaterial(*(object->getMaterial()));
                rc.render(object);
            }


    private:
        Object * object;
    };
}


#endif
