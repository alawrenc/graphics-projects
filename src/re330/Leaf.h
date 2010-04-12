#ifndef __Leaf_h__
#define __Leaf_h__

#include "Node.h"
#include "Matrix4.h"
#include "Camera.h"
#include "RenderContext.h"


namespace RE330
{
    class Leaf : public Node
    {
    public:
        Leaf();

        void updateMatrix();
        virtual void draw(Matrix4 m, RenderContext rc, Camera c);
    }
}

#endif
