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
        // initializes to identity matrix
        Node(): parent(NULL), localToWorldTransform(Matrix4::IDENTITY) {}
        ~Node(){ }

        Node * getParent()
            {
                return parent;
            }

        void setParent( Node * node)
            {
                parent = node;
            }

        virtual void draw(Matrix4 m, RenderContext& rc, Camera c) = 0;
        virtual void updateLocalToWorldTransform(Matrix4 m) = 0;
        Matrix4 getLocalToWorldTransform()
            {
                return Matrix4(localToWorldTransform);
            }

    protected:
        Node * parent;
        Matrix4 localToWorldTransform;
    };

}

#endif
