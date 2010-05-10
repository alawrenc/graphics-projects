#include "Shapes.h"
#include <cmath>
using namespace RE330;
#define PI 3.1415926535897932384626433

Object * Shapes::readObject(SceneManager* sm, std::string filename)
{
    int nVerts;
    float * vertices;
    float * normals;
    float * texcoords;
    int nIndices;
    int * indices;

    // read in an object from an obj file
    ObjReader::readObj(filename.c_str(), nVerts, &vertices, &normals,
                       &texcoords, nIndices, &indices );

    // normalize a obj files
    ObjReader::normalize(vertices, nVerts);

    Object* objIn = sm->createObject();

    // display read in object
    setupObject(objIn, nVerts, nIndices, vertices, NULL, normals, indices);

    return objIn;
}

// creates 3D shape from 2D bezier curve
// preconditions:  numEvalPoints >= 2
Object * Shapes::createBezierShape(SceneManager* sm,
                                   int numSegments,
                                   Vector3 cp [],
                                   int numEvalPoints,
                                   int numAnglesRotation)
{
    // precompute and store abcd for all segments
    float cubicCoefficient[numSegments][4][2];
    for (int s = 0; s < numSegments; s++)
    {
        // indices in cp: p0 = 3*s ... p3 = 3*s + 3
        int p0 = 3*s; int p1 = 3*s + 1;
        int p2 = 3*s + 2; int p3 = 3*s + 3;

        // a, 0=x, 1=y
        cubicCoefficient[s][0][0] = (-cp[p0][0] + 3*cp[p1][0] -
                                     3*cp[p2][0] + cp[p3][0]);
        cubicCoefficient[s][0][1] = (-cp[p0][1] + 3*cp[p1][1] -
                                     3*cp[p2][1] + cp[p3][1]);

        // b
        cubicCoefficient[s][1][0] = (3*cp[p0][0] - 6*cp[p1][0] + 3*cp[p2][0]);
        cubicCoefficient[s][1][1] = (3*cp[p0][1] - 6*cp[p1][1] + 3*cp[p2][1]);


        // c
        cubicCoefficient[s][2][0] = -3*cp[p0][0] + 3*cp[p1][0];
        cubicCoefficient[s][2][1] = -3*cp[p0][1] + 3*cp[p1][1];

        // d
        cubicCoefficient[s][3][0] = cp[p0][0];
        cubicCoefficient[s][3][1] = cp[p0][1];
    }
    //EVERYTHING OK
    // generate sample points with cubic polynomial form
    Vector4 curvePoints [numEvalPoints];
    float evalIncrement = 1.f / (numEvalPoints - 1);

    for(int i = 0; i < numEvalPoints; i++)
    {
        // gives numEvalPoints equally spaced t value in [0,1]
        float t = i * evalIncrement;
        std::cout << "t:" << t << std::endl;

        // gives int index in range from 0 to numSegments
        int s = 0;
        if (t == 1) {
            s = numSegments - 1;
        }
        else {
            s = std::floor(t * numSegments);
        }

        std::cout << "s:" << s << std::endl;

        // point = a*t^3 + b*t^2 + c*t + d
        float x = (cubicCoefficient[s][0][0]*t*t*t +
                   cubicCoefficient[s][1][0]*t*t +
                   cubicCoefficient[s][2][0]*t +
                   cubicCoefficient[s][3][0]);

        float y = (cubicCoefficient[s][0][1]*t*t*t +
                   cubicCoefficient[s][1][1]*t*t +
                   cubicCoefficient[s][2][1]*t +
                   cubicCoefficient[s][3][1]);

        float z = 0;
        curvePoints[i] = Vector4(x,y,z,0);
        std::cout << curvePoints[i] << std::endl;
    }

    // precalc and store rotation matrices
    Matrix4 rotations [numAnglesRotation];
    // rotate by 2*PI rad divided by numAnglesRotation each time
    double angleOfRotation = 2*PI / numAnglesRotation;
    //std::cout << "pi:" << pi << std::endl;
    //std::cout << "rotation:" << angleOfRotation << std::endl;
    for (int r = 0; r < numAnglesRotation; r++)
    {
        rotations[r] = Matrix4::rotateY(-r * angleOfRotation);
    }

    int numVertices = 3 * numEvalPoints * numAnglesRotation;
    std::cout << "Vertices:" << numVertices << std::endl;
    float bezier_vertices [numVertices];
    // for all curvePoints
    // rotate point around y-axis

    for (int point = 0; point < numEvalPoints; point++)
    {
        int pointIndex = 3 * numAnglesRotation * point;
        for (int rot = 0; rot < numAnglesRotation; rot++)
        {
            int startIndex = pointIndex + rot*3;
            Vector4 rotatedPoint = rotations[rot] * curvePoints[point];

            bezier_vertices[startIndex] = rotatedPoint[0];
            bezier_vertices[startIndex + 1] = rotatedPoint[1];
            bezier_vertices[startIndex + 2] = rotatedPoint[2];
            std::cout << "(" << bezier_vertices[startIndex] << ", " <<
                bezier_vertices[startIndex + 1] << ", " <<
                bezier_vertices[startIndex + 2] << ")" << std::endl;

        }
        std::cout << std::endl;
    }

    int numNormals = 6 * (numEvalPoints - 1) * numAnglesRotation;
    //std::cout << "Normals:" << numNormals << std::endl;
    float bezier_normals [numNormals];
    //int w = 0;
    for (int point = 0; point < (numEvalPoints - 1); point++)
    {
        int pointIndex = 3 * numAnglesRotation * point;
        Vector4 intP0 = Vector4(bezier_vertices[pointIndex*3],
                                bezier_vertices[pointIndex*3+ 1],
                                bezier_vertices[pointIndex*3+ 2],
                                1);

        Vector4 intP1 = Vector4(bezier_vertices[(pointIndex+1)*3],
                                bezier_vertices[(pointIndex+1)*3 + 1],
                                bezier_vertices[(pointIndex+1)*3 + 2],
                                1);
        Vector4 normal = intP1 - intP0;
        //std::cout << "normal:" << normal << std::endl;
        Vector4 tangent = Vector4(-normal[1],
                                  normal[0],
                                  0,
                                  1);
        pointIndex = 2 * pointIndex;
        for (int rot = 0; rot < numAnglesRotation; rot++)
        {
            int startIndex = 6*rot + pointIndex;
            //calculating normals
            Vector4 tangent1 = rotations[rot] * tangent;
            Vector4 tangent2 = rotations[rot] * tangent;
            Vector4 average = tangent1 + tangent2;
            average.normalize();
            //setting normals
            bezier_normals[startIndex] = average[0];
            bezier_normals[startIndex + 1] = average[1];
            bezier_normals[startIndex + 2] = average[2];

            bezier_normals[startIndex + 3] = average[0];
            bezier_normals[startIndex + 4] = average[1];
            bezier_normals[startIndex + 5] = average[2];

            //std::cout << "(" << bezier_normals[startIndex] << ", " <<
            //	bezier_normals[startIndex + 1] << ", " <<
            //bezier_normals[startIndex + 2] << ")" << std::endl;
            //std::cout << "(" << bezier_normals[startIndex + 3] << ", " <<
            //	bezier_normals[startIndex + 4] << ", " <<
            //	bezier_normals[startIndex + 5] << ")" << std::endl;
            //w = w + 6;
            //std::cout << w << std::endl;


        }
    }

    //texture coordinates
    int numTextureCoords = 2 * numEvalPoints * numAnglesRotation;
    float bezierTextureCoords[numTextureCoords];
    for (int point = 0; point < numEvalPoints; point++)
    {
        int pointIndex = 2 * numAnglesRotation * point;
        for (int rot = 0; rot < numAnglesRotation; rot++)
        {
            int startIndex = pointIndex + rot*2;
            if (rot%2 == 0) {
                bezierTextureCoords[startIndex] = 1;
            }
            else{
                bezierTextureCoords[startIndex] = 0;
            }
            if (point%2 == 0) {
                bezierTextureCoords[startIndex + 1] = 1;
            }
            else {
                bezierTextureCoords[startIndex + 1] = 0;
            }
        }
    }



    // generate indices
    // each point is responsible for two triangles extending away from it that
    // form a square together
    int numIndices = 12 * (numEvalPoints - 1) * numAnglesRotation;
    int bezierIndices[numIndices];
    for (int point = 0; point < (numEvalPoints - 1); point++)
    {
        int pointIndex = 12 * numAnglesRotation * point;
        for (int rot = 0; rot < numAnglesRotation; rot++)
        {
            int startIndex = 12 * rot + pointIndex;
            if (rot < (numAnglesRotation - 1)) {
                //front face
                bezierIndices[startIndex] = startIndex / 12;
                bezierIndices[startIndex + 1] = (startIndex / 12) + 1;
                bezierIndices[startIndex + 2] = ((startIndex / 12) +
                                                 numAnglesRotation + 1);

                bezierIndices[startIndex + 3] = startIndex / 12;
                bezierIndices[startIndex + 4] = ((startIndex / 12) +
                                                 numAnglesRotation + 1);
                bezierIndices[startIndex + 5] = ((startIndex / 12) +
                                                 numAnglesRotation);
                //back face
                bezierIndices[startIndex + 6] = startIndex / 12;
                bezierIndices[startIndex + 7] = ((startIndex / 12) +
                                                 numAnglesRotation + 1);
                bezierIndices[startIndex + 8] = (startIndex / 12) + 1;

                bezierIndices[startIndex + 9] = startIndex / 12;
                bezierIndices[startIndex + 10] = ((startIndex / 12) +
                                                  numAnglesRotation);
                bezierIndices[startIndex + 11] = ((startIndex / 12) +
                                                  numAnglesRotation + 1);

            }
            else {
                //front face
                bezierIndices[startIndex] = startIndex / 12;
                bezierIndices[startIndex + 1] = (pointIndex / 12);
                bezierIndices[startIndex + 2] = ((pointIndex / 12) +
                                                 numAnglesRotation);

                bezierIndices[startIndex + 3] = startIndex / 12;
                bezierIndices[startIndex + 4] = ((pointIndex / 12) +
                                                 numAnglesRotation);
                bezierIndices[startIndex + 5] = ((startIndex / 12) +
                                                 numAnglesRotation);
                //back face
                bezierIndices[startIndex + 6] = startIndex / 12;
                bezierIndices[startIndex + 7] = ((pointIndex / 12) +
                                                 numAnglesRotation);
                bezierIndices[startIndex + 8] = (pointIndex / 12);

                bezierIndices[startIndex + 9] = startIndex / 12;
                bezierIndices[startIndex + 10] = ((startIndex / 12) +
                                                  numAnglesRotation);
                bezierIndices[startIndex + 11] = ((pointIndex / 12) +
                                                  numAnglesRotation);

            }
        }
    }

    Object * bezier = sm->createObject();
    setupObjectTexture(bezier, numVertices/3, numIndices, bezier_vertices,
                       NULL, bezier_normals, bezierTextureCoords, bezierIndices);
    //std::cout << "test" << std::endl;
    return bezier;
}

void Shapes::setupObjectTexture(Object* obj, int nVerts, int nIndices,
                                float* v, float* c, float* n, float* t, int* i)
{
    VertexData& vd = obj->vertexData;
    // Specify the elements of the vertex data:
    // - one element for vertex positions
    vd.vertexDeclaration.addElement(0, 0, 3, 3*sizeof(float),
                                    RE330::VES_POSITION);
    vd.createVertexBuffer(0, nVerts*3*sizeof(float), (unsigned char*)v);

    // - one element for vertex colors
    if (c != NULL)
    {
        vd.vertexDeclaration.addElement(1, 0, 3, 3*sizeof(int),
                                        RE330::VES_DIFFUSE);
        vd.createVertexBuffer(1, nVerts*3*sizeof(float), (unsigned char*)c);
    }
    if (n != NULL)
    {
        vd.vertexDeclaration.addElement(1, 0, 3, 3*sizeof(float),
                                        RE330::VES_NORMAL);
        vd.createVertexBuffer(1, nVerts*3*sizeof(float),
                              (unsigned char*)n);
    }

    vd.vertexDeclaration.addElement(2, 0, 2, 2*sizeof(float),
                                    RE330::VES_TEXTURE_COORDINATES);
    vd.createVertexBuffer(2, nVerts*2*sizeof(float), (unsigned char*)t);

    // Create the buffers and load the data
    vd.createIndexBuffer(nIndices, i);

    obj->computeBoundingSphere(nVerts*3, v);
}


void Shapes::setupObject(Object* obj, int nVerts, int nIndices,
                         float* v, float* c, float* n, int* i)
{
    VertexData& vd = obj->vertexData;

    // Specify the elements of the vertex data:
    // - one element for vertex positions
    vd.vertexDeclaration.addElement(0, 0, 3, 3*sizeof(float),
                                    RE330::VES_POSITION);
    vd.createVertexBuffer(0, nVerts*3*sizeof(float), (unsigned char*)v);

    // - one element for vertex colors
    if (c != NULL)
    {
        vd.vertexDeclaration.addElement(1, 0, 3, 3*sizeof(int),
                                        RE330::VES_DIFFUSE);
        vd.createVertexBuffer(1, nVerts*3*sizeof(float), (unsigned char*)c);
    }
    if (n != NULL)
    {
        vd.vertexDeclaration.addElement(1, 0, 3, 3*sizeof(float),
                                        RE330::VES_NORMAL);
        vd.createVertexBuffer(1, nVerts*3*sizeof(float),
                              (unsigned char*)n);
    }

    // Create the buffers and load the data
    vd.createIndexBuffer(nIndices, i);

    obj->computeBoundingSphere(nVerts*3, v);

    // if(n) delete[] n;
    // if(c) delete[] c;
    // delete[] v;
    // delete[] i;
}
