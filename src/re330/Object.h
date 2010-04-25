#ifndef __Object_h__
#define __Object_h__

#include "RE330_global.h"
#include "VertexData.h"
#include "Matrix4.h"
#include "Material.h"

namespace RE330
{
    /** This class provides an abstraction of scene objects.
    @remarks
        The SceneManager creates and deletes objects. Vertex data is stored
        in the VertexData member.
    */
    class RE330_EXPORT Object
    {
    public:

        inline void setTransformation(const Matrix4 &t) { mTransformation = t; }

        inline Matrix4 getTransformation() const { return mTransformation; }

        inline Material* getMaterial() { return &objectMaterial; }

        inline void setMaterial(const Material &m) { objectMaterial = m; }

        inline int getRadius() { return radius; }

        inline Vector3 getCenter() { return center; }

        void printTransformation()
        {
            printf("(%f %f %f %f)\n", mTransformation[0], mTransformation[1],
                   mTransformation[2], mTransformation[3]);
            printf("(%f %f %f %f)\n", mTransformation[4], mTransformation[5],
                   mTransformation[6], mTransformation[7]);
            printf("(%f %f %f %f)\n", mTransformation[8], mTransformation[9],
                   mTransformation[10], mTransformation[11]);
            printf("(%f %f %f %f)\n", mTransformation[12], mTransformation[13],
                   mTransformation[14], mTransformation[15]);
            fflush(NULL);
        }

        VertexData vertexData;

        void computeBoundingSphere(int numVertElem, float* verts);

        static void findMinMaxVectors(float *vertices, int numVertices,
                                      Vector3 *minVector, Vector3 *maxVector);


    protected:

        Object()
        {
            mTransformation = Matrix4::IDENTITY;
            objectMaterial = Material();
        }

        Matrix4 mTransformation;

        Material objectMaterial;

        int radius;
        Vector3 center;

        friend class SceneManager;
    };

}

#endif
