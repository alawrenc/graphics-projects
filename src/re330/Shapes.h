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

        static Object *createSheet(SceneManager* sm);

        static Object *createHouse(SceneManager* sm);

        static Object *createQuadHouses(SceneManager* sm);

        static Object *createSphere(SceneManager* sm,
                                    float height, int slices, int points,
                                    const int num_colors,
                                    float color_list[][3],
                                    bool random_colors);

        static Object *createCone(SceneManager* sm, float height,
                                  float base_radius, int base_points,
                                  const int num_colors,
                                  float color_list[][3],
                                  bool random_colors);

        static Object *createBox(SceneManager* sm, float height,
                                 float width, float depth,
                                 const int num_colors,
                                 float color_list[][3],
                                 bool random_colors);

        // Create the cone arrays
        static float* coneVertices(float height, float base_radius,
                                   int base_points, const int num_colors);
        static float* coneColors(int base_points, const int num_colors,
                                 float color_list[][3]);
        static int* coneIndices(int base_points, const int num_colors,
                                bool random_colors);

        // Create the sphere arrays
        static float* sphereVertices(float height, int slices, int points,
                                     const int num_colors);
        static float* sphereColors(int slices, int points,
                                   const int num_colors,
                                   float color_list[][3]);
        static int* sphereIndices(int slices, int points,
                                  const int num_colors, bool random_colors);

        // Create the box arrays
        static float* boxVertices(float height, float width, float depth,
                                  const int num_colors);
        static float* boxColors(const int num_colors,
                                float color_list[][3]);
        static int* boxIndices(const int num_colors, bool random_colors);

        static void setupObject(Object* obj, int nVerts, int nIndices,
                                float* v, float* c, float* normals, int* i);

        static void setupObjectTexture(Object* obj, int nVerts, int nIndices,
                                       float* v, float* c, float* normals,
                                       float* t, int* i);
    };

}

#endif
