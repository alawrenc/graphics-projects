#ifndef __Shapes_h__
#define __Shapes_h__

#include <time.h>
#include <stdlib.h>
#include <math.h>
#include "Object.h"
#include "SceneManager.h"
#include "ObjReader.h"

namespace RE330
{
    class RE330_EXPORT Shapes
    {

    public:
        /** These methods create basic shapes.
         */
        static Object *createBezierShape(SceneManager* sm,
                                         int numSegments,
                                         Vector3 cp[],
                                         int numEvalPoints,
                                         int numAnglesRotation);

        static Object *readObject(SceneManager* sm, std::string filename);

        static void setupObject(Object* obj, int nVerts, int nIndices,
                                float* v, float* c, float* normals, int* i);

        static void setupObjectTexture(Object* obj, int nVerts, int nIndices,
                                       float* v, float* c, float* normals,
                                       float* t, int* i);
    };

}

#endif
