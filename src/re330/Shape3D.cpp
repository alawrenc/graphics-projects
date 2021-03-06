#include "Shape3D.h"
using namespace RE330;

void Shape3D::draw(Matrix4 m, RenderContext &rc, Camera c)
{
    localToWorldTransform = m;
    viewTransform = c.getViewMatrix() * m * object->getTransformation();
    rc.setModelViewMatrix(viewTransform);
    //object->setMaterial(*(object->getMaterial()));
    if (rc.culling)
    {
        if (objectInView(c)) // and CULLING
        {
            rc.render(object);
        }
    }
    else
    {
        rc.render(object);
    }
}

bool Shape3D::objectInView(Camera c)
{
    computeBoundingSphere(c);
    return (detectSphereIntersection(c) <= 0.0f);
}

//returns 1 if outside frustum, 0 if intersects, -1 if inside
int Shape3D::detectSphereIntersection(Camera c)
{
// Precompute the normal n and value d for each of the six planes.
//  d = p.n   n = a x b
// p is a point on plane, n is unit normal

    float zNear = -c.getNearPlane();
    float zFar = -c.getFarPlane();
    float theta = c.getVerticalFOV() / 2.0f;

    //symmetrical view volume
    float yTopFar = zFar*tan(theta);
    float yBotFar = -yTopFar;
    float xLeftFar = -yTopFar;
    float xRightFar = yTopFar;

    float yTopNear = zNear*tan(theta);
    float yBotNear = -yTopNear;
    float xLeftNear = -yTopNear;
    float xRightNear = yTopNear;

    // every plane defined by two vectors. need to find normal
    // near plane
    Vector3 topFront (xLeftNear - xRightNear, 0, 0);
    Vector3 leftFront (0, yTopNear - yBotNear, 0);
    Vector3 nearPlaneNormal = leftFront * topFront;
    nearPlaneNormal.normalize();
    Vector3 topFrontLeftPoint(xLeftNear, yTopNear, zNear);
    float nearPlaneD = topFrontLeftPoint ^ nearPlaneNormal;

    //top plane
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
    Vector3 rightPlaneNormal = backRight * botRight;
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

void Shape3D::computeBoundingSphere(Camera c)
{
    // apply transform to center vector
    Vector4 center4 = (viewTransform * Vector4(object->getCenter(), 1));
    boundingSphereCenter = center4.asVector3();

    // pull scaling factors from transform and apply to radius
    boundingSphereRadius = (object->getRadius() *
                            viewTransform[0] *
                            viewTransform[5] *
                            viewTransform[10]);
}

