#ifndef __Shape3D_h__
#define __Shape3D_h__

#include "Leaf.h"
#include "Matrix4.h"
#include "Vector3.h"
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
                initialBoundsComputed = false;
                //lastObjectTransform = object->getTransformation();
            }

        void draw(Matrix4 m, RenderContext &rc, Camera c);

    private:
        int detectSphereIntersection(Camera c);
        bool objectInView(Camera c);
        void computeBoundingSphere(Camera c);

        // modifies Vector3 params to hold min/max xyz values
        void findMinMaxVectors(unsigned char *vertices, int numVertices,
                               Vector4 *minVector, Vector4 *maxVector);

        Object * object;
        Vector3 boundingSphereCenter;
        Matrix4 lastObjectTransform;
        float boundingSphereRadius;
        bool initialBoundsComputed;
    };
}


#endif
