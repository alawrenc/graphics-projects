#include "Object.h"
#include <math.h>
using namespace RE330;

#include <limits>
using namespace std;


void Object::computeBoundingSphere(int numVertElem, float* vertices)
{
    Vector3 min;
    Vector3 max;
    findMinMaxVectors(vertices, numVertElem, &min, &max);

    int xDiff = max[0] - min[0];
    int yDiff = max[1] - min[1];
    int zDiff = max[2] - min[2];

    radius = std::max(xDiff, std::max(yDiff, zDiff));

    float xCenter = xDiff / 2.0f + min[0];
    float yCenter = yDiff / 2.0f + min[1];
    float zCenter = zDiff / 2.0f + min[2];

    center = Vector3(xCenter, yCenter, zCenter);
}

void Object::findMinMaxVectors(float *vertices, int numVertices,
                               Vector3 *minVector, Vector3 *maxVector)
{
    //initialize min/max values to limits
    float minX = numeric_limits<float>::max();
    float minY = numeric_limits<float>::max();
    float minZ = numeric_limits<float>::max();

    float maxX = numeric_limits<float>::min();
    float maxY = numeric_limits<float>::min();
    float maxZ = numeric_limits<float>::min();

    int remainder = NULL;
    float value = NULL;

    // iterate through all values to find min for each: xyz
    for (int count = 0; count < numVertices; count++)
    {
        value = vertices[count];
        remainder = count % 3;
        if (remainder == 0)
        {
            maxX = std::max(maxX, value);
            minX = std::min(minX, value);
        }

        else if (remainder == 1)
        {
            maxY = std::max(maxY, value);
            minY = std::min(minY, value);
        }

        else if (remainder == 2)
        {
            maxZ = std::max(maxZ, value);
            minZ = std::min(minZ, value);
        }
    }

    *minVector = Vector3(minX, minY, minZ);
    *maxVector = Vector3(maxX, maxY, maxZ);
}

