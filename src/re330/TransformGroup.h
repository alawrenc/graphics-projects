#ifndef __TransformGroup_h__
#define __TransformGroup_h__

// Stores additional transformation M
//Transformation applies to subtree below node
//Computer-to-world transform

#include "NodeGroup.h"
#include "RenderContext.h"
#include "Matrix4.h"

namespace RE330
{
    class TransformGroup : public NodeGroup
    {

    public:
        TransformGroup(Matrix4 transform): local(transform) {}

        void draw(Matrix4 m, RenderContext& rc, Camera c)
            {
                //C_new = C*M;
                end = children.end();
                for (it = children.begin(); it != end; it ++)
                {
                    (*it)->draw(m*local, rc, c);
                }

            }

        Matrix4 getLocalTransform()
            {
                return Matrix4(local);
            }

        void setLocalTransform(Matrix4 m)
            {
                local = m;
                updateLocalToWorldTransform(local);
            }

        void updateLocalToWorldTransform(Matrix4 m)
            {
                // update my localToWorldTransform
                localToWorldTransform = m * local;
                // update all children with my new transform
                updateChildren();
            }

    protected:
        Matrix4 local;
        void updateChildren()
            {
                end = children.end();
                for (it = children.begin(); it != end; it ++)
                {
                    (*it)->updateLocalToWorldTransform(localToWorldTransform);
                }
            }

    };
}

#endif
