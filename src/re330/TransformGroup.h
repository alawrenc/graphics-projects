#ifndef __TransformGroup_h__
#define __TransformGroup_h__

// Stores additional transformation M
//Transformation applies to subtree below node
//Computer-to-world transform

#include "NodeGroup.h"
#include "RenderContext.h"

namespace RE330
{
    class TransformGroup : public NodeGroup
    {

    public:
        TransformGroup();

        void draw(Matrix4 m, const RenderContext& rc, Camera c)
            {
                //C_new = C*M;
                end = children.end();
                for (it = children.begin(); it != end; it ++)
                {
                    draw(m, rc, c);
                }

            }

        void updateMatrix()
            {

            }
    };
}

#endif
