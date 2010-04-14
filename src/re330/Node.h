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

        Node(): parent(NULL), localToWorldTransfrom(Matrix4()) {}

        void updateMatrix()
            {
                if(parent)
                {
                    localToWorldTransform = (parent.localToWorldTransform *
                                             localToWorldTransform);
                }
            }

        Node * getParent()
            {
                return parent;
            }

        void setParent(*Node)
            {
                parent = Node;
            }

        virtual void draw(Matrix4 m, RenderContext rc, Camera c);

    protected:
        Matrix4 localToWorldTransform;
        private * Node parent;
    };

}

#endif
