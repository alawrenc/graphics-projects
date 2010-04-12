#ifndef __Node_h__
#define __Node_h__

#include "Matrix4.h"
#include "Camera.h"
#include "RenderContext.h"

namespace RE330
{
    class Node
    {

    public:

        Node();

        virtual void updateMatrix();
        Node getParent();
        virtual void draw(Matrix4 m, RenderContext rc, Camera c);

    private:
        Matrix4 localToWorldTransform;
    };

}

#endif
