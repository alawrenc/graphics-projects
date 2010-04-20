#ifndef __Node_h__
#define __Node_h__

#include "Matrix4.h"
#include "Camera.h"
#include "GLRenderContext.h"

namespace RE330
{
    class Node
    {

    public:
        // initializes to identity matrix
        Node(): parent(NULL), localToWorldTransform(Matrix4()) {}

        void updateMatrix()
            {
                if(parent)
                {
                    localToWorldTransform = ((*parent).localToWorldTransform *
                                             localToWorldTransform);
                }
            }

        Node * getParent()
            {
                return parent;
            }

        void setParent( Node * node)
            {
                parent = node;
            }

        virtual void draw(Matrix4 m, GLRenderContext rc, Camera c) = 0;

    protected:
        Node * parent;
        Matrix4 localToWorldTransform;
    };

}

#endif
