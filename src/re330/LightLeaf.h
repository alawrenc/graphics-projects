#ifndef __LightLeaf_h__
#define __LightLeaf_h__

#include "Leaf.h"
#include "Matrix4.h"
#include "Camera.h"
#include "RenderContext.h"
#include "Light.h"


namespace RE330
{
    class LightLeaf : public Leaf
    {

    public:
        LightLeaf();

        void updateMatrix();
        void draw(Matrix4 m, RenderContext rc, Camera c);

    private:
        Light * light;

    }
}

#endif
