#include "Shape3D.h"
#include <math.h>
using namespace RE330;

#include <limits>
using namespace std;

void Shape3D::draw(Matrix4 m, RenderContext &rc, Camera c)
{
    rc.setModelViewMatrix(c.getViewMatrix() * m *
                          object->getTransformation());
    //object->setMaterial(*(object->getMaterial()));
    if (objectInView(c)) // and CULLING
    {
        rc.render(object);
    }
}

bool Shape3D::objectInView(Camera c)
{
    computeBoundingSphere();
    return (detectSphereIntersection(c) <= 0);
}

//returns 1 if outside frustum, 0 if intersects, -1 if inside
int Shape3D::detectSphereIntersection(Camera c)
{
// Precompute the normal n and value d for each of the six planes.
//  d = p.n   n = a x b
// p is a point on plane, n is unit normal

    float zNear = c.getNearPlane();
    float zFar = c.getFarPlane();
    float theta = c.getVerticalFOV() / 2.0f;

    float yTopFar = zFar*tan(theta);
    float yBotFar = -yTopFar;
    float xLeftFar = yTopFar;
    float xRightFar = -xLeftFar;

    float yTopNear = zNear*tan(theta);
    float yBotNear = -yTopNear;
    float xLeftNear = yTopNear;
    float xRightNear = -xLeftNear;

    // every plane defined by two vectors. need to find normal
    // near plane
    Vector3 topFront (xLeftNear - xRightNear, 0, 0);
    Vector3 leftFront (0, yTopNear - yBotNear, 0);
    Vector3 nearPlaneNormal = topFront * leftFront;
    nearPlaneNormal.normalize();
    Vector3 topFrontLeftPoint(xLeftNear, yTopNear, zNear);
    float nearPlaneD = topFrontLeftPoint ^ nearPlaneNormal;

    // top plane
    Vector3 leftTopSide (xLeftNear - xLeftFar,
                         yTopNear - yTopFar,
                         zNear - zFar);
    Vector3 topPlaneNormal = topFront * leftTopSide;
    topPlaneNormal.normalize();
    float topPlaneD = topFrontLeftPoint ^ topPlaneNormal;

    // left plane
    Vector3 leftPlaneNormal = leftTopSide * leftFront;
    leftPlaneNormal.normalize();
    float leftPlaneD = topFrontLeftPoint ^ leftPlaneNormal;

    // far plane
    Vector3 botBack (xRightFar - xLeftFar, 0, 0);
    Vector3 backRight (0, yBotFar - yTopFar, 0);
    Vector3 farPlaneNormal = botBack * backRight;
    farPlaneNormal.normalize();
    Vector3 botBackRightPoint (xRightFar, yBotFar, zFar);
    float farPlaneD = botBackRightPoint ^ farPlaneNormal;

    // bottom plane
    Vector3 botRight (xRightFar - xRightNear, yBotFar - yBotNear, zFar - zNear);
    Vector3 botPlaneNormal = botRight * botBack;
    botPlaneNormal.normalize();
    float botPlaneD = botBackRightPoint ^ botPlaneNormal;

    // right plane
    Vector3 rightPlaneNormal = botRight*backRight;
    rightPlaneNormal.normalize();
    float rightPlaneD = botBackRightPoint ^ rightPlaneNormal;

// Given a sphere with center x and radius r
//  For each plane:
// dist(x)= x.n - d
// if dist(x) > r: sphere is outside! (no need to continue loop)
// if dist(x) < -r: add 1 to count
    float distNearPlane = (boundingSphereCenter^nearPlaneNormal) - nearPlaneD;
    float distTopPlane = (boundingSphereCenter^topPlaneNormal) - topPlaneD;
    float distLeftPlane = (boundingSphereCenter^leftPlaneNormal) - leftPlaneD;
    float distFarPlane = (boundingSphereCenter^farPlaneNormal) - farPlaneD;
    float distBotPlane = (boundingSphereCenter^botPlaneNormal) - botPlaneD;
    float distRightPlane = (boundingSphereCenter^rightPlaneNormal) - rightPlaneD;

    // if we are outside any plane completely we're outside all
    if (distNearPlane > boundingSphereRadius ||
        distTopPlane > boundingSphereRadius ||
        distLeftPlane > boundingSphereRadius ||
        distFarPlane > boundingSphereRadius ||
        distBotPlane > boundingSphereRadius ||
        distRightPlane > boundingSphereRadius)
    {
        return 1;
    }

    // if we're inside all planes
    if (distNearPlane < -boundingSphereRadius &&
        distTopPlane < -boundingSphereRadius &&
        distLeftPlane < -boundingSphereRadius &&
        distFarPlane < -boundingSphereRadius &&
        distBotPlane < -boundingSphereRadius &&
        distRightPlane < -boundingSphereRadius)
    {
        return -1;
    }
    // we intersect a plane
    return 0;

// If we made it through the loop, check the count:
// if the count is 6, the sphere is completely inside
// otherwise the sphere intersects the frustum
//  (can use a flag instead of a count)
}

void Shape3D::computeBoundingSphere()
{
    // only run expensive min/max finding once
    if (!initialBoundsComputed)
    {
        int * vertices = object->vertexData.getIndexBuffer();
        int numVertices = object->vertexData.getIndexCount();
        Vector3 min;
        Vector3 max;
        findMinMaxVectors(vertices, numVertices, &min, &max);
        int xDiff = max[0] - min[0];
        int yDiff = max[1] - min[1];
        int zDiff = max[2] - min[2];

        boundingSphereRadius = std::max(xDiff, std::max(yDiff, zDiff));

        float xCenter = xDiff / 2.0f + min[0];
        float yCenter = yDiff / 2.0f + min[1];
        float zCenter = zDiff / 2.0f + min[2];

        boundingSphereCenter = Vector3(xCenter, yCenter, zCenter);
        initialBoundsComputed = true;
    }

    // only update center and radius if the transformation changed
    //else if (lastObjectTransform != object->getTransformation())
    //{
        // apply transform to center vector if it's changed
    //boundingSphereCenter = boundingSphereCenterobject->getTransformation();
        // apply transform to unit vectors, apply unit vectors to previous radius
        // ???
        //}
}

void Shape3D::findMinMaxVectors(int *vertices, int numVertices,
                                Vector3 *minVector, Vector3 *maxVector)
{
    //initialize min/max values to limits
    int minX = numeric_limits<int>::max();
    int minY = numeric_limits<int>::max();
    int minZ = numeric_limits<int>::max();

    int maxX = numeric_limits<int>::min();
    int maxY = numeric_limits<int>::min();
    int maxZ = numeric_limits<int>::min();

    int remainder = NULL;
    int value = NULL;

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
