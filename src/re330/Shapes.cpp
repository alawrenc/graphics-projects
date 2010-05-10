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
    setupObject(bezier, numVertices/3, numIndices, bezier_vertices,
                bezierTextureCoords, bezier_normals, bezierIndices);
    //std::cout << "test" << std::endl;
    return bezier;
}

Object * Shapes::createSheet(SceneManager* sm)
{
    Object* sheet = sm->createObject();
    float sheet_v [] = { -2,-2,2, -2,-2,-2, 2,-2,-2, 2,-2,2};
    float sheet_c [] = { 1,0,0, 1,1,1, 1,1,1, 1,0,0 };
    int sheet_i [] = { 0,2,1, 0,3,2 };
    int nVerts = 4;
    int nIndices = 6;

    float textCoords[] = {0,0, 0,1, 1,1, 1,0};

    setupObjectTexture(sheet, nVerts, nIndices, sheet_v, NULL, NULL, textCoords, sheet_i);

    return sheet;

}
Object * Shapes::createHouse(SceneManager* sm)
{
    Object* house = sm->createObject();

    float house_v [] = {
        -4,-4,4, 4,-4,4, 4,4,4, -4,4,4,      // front face
        -4,-4,-4, -4,-4,4, -4,4,4, -4,4,-4,  // left face
        4,-4,-4,-4,-4,-4, -4,4,-4, 4,4,-4,   // back face
        4,-4,4, 4,-4,-4, 4,4,-4, 4,4,4,      // right face
        4,4,4, 4,4,-4, -4,4,-4, -4,4,4,      // top face
        -4,-4,4, -4,-4,-4, 4,-4,-4, 4,-4,4,  // bottom face
        -20,-4,20, 20,-4,20, 20,-4,-20, -20,-4,-20,// ground floor
        -4,4,4, 4,4,4, 0,8,4,                        // roof
        4,4,4, 4,4,-4, 0,8,-4, 0,8,4,
        -4,4,4, 0,8,4, 0,8,-4, -4,4,-4,
        4,4,-4, -4,4,-4, 0,8,-4
    };
    float house_c [] = {
        1,0,0, 1,0,0, 1,0,0, 1,0,0,	// front face
        0,1,0, 0,1,0, 0,1,0, 0,1,0,	// left face
        1,0,0, 1,0,0, 1,0,0, 1,0,0,	// back face
        0,1,0, 0,1,0, 0,1,0, 0,1,0,	// right face
        0,0,1, 0,0,1, 0,0,1, 0,0,1,	// top face
        0,0,1, 0,0,1, 0,0,1, 0,0,1,	// bottom face

        0,0.5,0, 0,0.5,0, 0,0.5,0, 0,0.5,0,	// ground floor
        0,0,1, 0,0,1, 0,0,1,					// roof
        1,0,0, 1,0,0, 1,0,0, 1,0,0,
        0,1,0, 0,1,0, 0,1,0, 0,1,0,
        0,0,1, 0,0,1, 0,0,1
    };
    int house_i [] = {
        0,2,3, 0,1,2,			// front face
        4,6,7, 4,5,6,			// left face
        8,10,11, 8,9,10,		// back face
        12,14,15, 12,13,14,	// right face
        16,18,19, 16,17,18,	// top face
        20,22,23, 20,21,22,	// bottom face

        24,26,27, 24,25,26,	// ground floor
        28,29,30,				// roof
        31,33,34, 31,32,33,
        35,37,38, 35,36,37,
        39,40,41
    };

    int nVerts = 42;
    int nIndices = 60;

    setupObject(house, nVerts, nIndices, house_v, house_c, NULL, house_i);

    return house;
}

Object* Shapes::createSphere(SceneManager* sm, float height, int slices,
                             int points, const int num_colors,
                             float color_list[][3], bool random_colors)
{
    Object *sphere = sm->createObject();
    float* sphere_v = sphereVertices(height, slices, points, num_colors);
    float* sphere_c = sphereColors(slices, points, num_colors, color_list);
    int* sphere_i = sphereIndices(slices, points, num_colors, random_colors);
    int nVerts = (slices + 1) * points * num_colors;
    int nIndices = slices * points * 2 * 3;
    setupObject(sphere, nVerts, nIndices, sphere_v, sphere_c, NULL, sphere_i);
    return sphere;
}

Object* Shapes::createCone(SceneManager* sm, float height, float base_radius,
                           int base_points, const int num_colors,
                           float color_list[][3], bool random_colors)
{
    Object *cone = sm->createObject();
    float* cone_v = coneVertices(height, base_radius, base_points, num_colors);
    float* cone_c = coneColors(base_points, num_colors, color_list);
    int* cone_i = coneIndices(base_points, num_colors, random_colors);
    int nVerts = (1 + base_points) * num_colors;
    int nIndices = 3 * base_points;
    setupObject(cone, nVerts, nIndices, cone_v, cone_c, NULL, cone_i);
    return cone;
}

Object* Shapes::createBox(SceneManager* sm, float height, float width,
                          float depth, const int num_colors,
                          float color_list[][3], bool random_colors)
{
    Object *box = sm->createObject();
    float* box_v = boxVertices(height, width, depth, num_colors);
    float* box_c = boxColors(num_colors, color_list);
    int* box_i = boxIndices(num_colors, random_colors);
    int nVerts = 8 * num_colors;
    int nIndices = 3 * 12;
    float textCoords[] = {1,1, 0,1, 1,1, 0,1,
                          1,0, 0,0, 1,0, 0,0,};
    //setupObject(box, nVerts, nIndices, box_v, box_c, NULL, box_i);
    setupObjectTexture(box, nVerts, nIndices, box_v, box_c, NULL, textCoords, box_i);
    return box;
}

float* Shapes::coneVertices(float height, float base_radius, int base_points,
                            const int num_colors)
{
    float* array = new float[3 * num_colors * (1 + base_points)];
    // first make two top vertices for the
    int index = 0;
    for (int i = 0; i < num_colors; i++) {
        array[index++] = 0;
        array[index++] = height;
        array[index++] = 0;
    }
    // now do the base
    float theta;
    for (int i = 0; i < base_points; i++) {
        theta = 2 * M_PI / base_points * i;
        for (int j = 0; j < num_colors; j++) {
            array[index++] = base_radius * cos(theta);
            array[index++] = -height;
            array[index++] = base_radius * sin(theta);
        }
    }
    return array;
}

float* Shapes::coneColors(int base_points, const int num_colors,
                          float color_list[][3])
{
    float* array = new float[3 * num_colors * (1 + base_points)];
    int index = 0;
    // for the top point(s)
    for (int i = 0; i < num_colors; i++) {
        array[index++] = color_list[i][0];
        array[index++] = color_list[i][1];
        array[index++] = color_list[i][2];
    }
    for (int i = 0; i < base_points; i++) {
        for (int j = 0; j < num_colors; j++) {
            array[index++] = color_list[j][0];
            array[index++] = color_list[j][1];
            array[index++] = color_list[j][2];
        }
    }
    return array;
}
int* Shapes::coneIndices(int base_points, const int num_colors,
                         bool random_colors)
{
    srand(time(NULL));
    // connect the top point to adjacent base points.
    int* array = new int[3 * base_points];
    // color_index keeps track of the color of the current triangle
    int color_index = 0;
    if (random_colors) {
        color_index = rand() % num_colors;
    }
    int index = 0;
    int i;
    for (i = 0; i < base_points - 1; i++) {
        // top point
        array[index++] = color_index;
        // base points
        array[index++] = num_colors + color_index + num_colors * i;
        array[index++] = 2 * num_colors + color_index + num_colors * i;
        if (random_colors) {
            color_index = rand() % num_colors;
        } else {
            color_index++;
            color_index %= num_colors;
        }
    }
    // finally connect the last base point to the first base point.
    array[index++] = color_index;
    array[index++] = num_colors + color_index + num_colors * i;
    array[index++] = num_colors + color_index;
    return array;
}
float* Shapes::sphereVertices(float height, int slices, int points,
                              const int num_colors)
{
    float* array = new float[(slices + 1) * points * 3 * num_colors];
    int index = 0;
    float theta, phi;
    for(int s = 0; s <= slices; s++) {
        for(int p = 0; p < points; p++) {
            theta = 2*M_PI/points*p;
            phi = M_PI/slices*s;
            for (int i = 0; i < num_colors; i++) {
                array[index++] = height * cos(theta) * sin(phi);
                array[index++] = height * sin(theta) * sin(phi);
                array[index++] = height * cos(phi);
            }
        }
    }
    return array;
}
float* Shapes::sphereColors(int slices, int points, const int num_colors,
                            float color_list[][3])
{
    float* array = new float[(slices + 1) * points * 3 * num_colors];
    int index;
    index = 0;
    for(int i = 0; i <= slices; i++) {
        for(int j = 0; j < points; j++) {
            for (int k = 0; k < num_colors; k++) {
                array[index++] = color_list[k][0];
                array[index++] = color_list[k][1];
                array[index++] = color_list[k][2];
            }
        }
    }
    return array;
}
int* Shapes::sphereIndices(int slices, int points, const int num_colors,
                           bool random_colors)
{
    srand(time(NULL));
    int* array = new int[2 * (slices) * points * 3];
    int index = 0;
    int color_index = 0;
    if (random_colors) {
        color_index = rand() % num_colors;
    }
    for (int s = 0; s < slices; s++) {
        for (int p = 0; p < points; p++) {
            array[index++] = num_colors * (s * points + p + 1) + color_index;
            array[index++] = num_colors * (s * points + p) + color_index;
            array[index++] = num_colors * ((s + 1) * points + p) + color_index;
            if (random_colors) {
                color_index = rand() % num_colors;
            } else {
                color_index++;
                color_index %= num_colors;
            }
            array[index++] = num_colors * ((s + 1) * points + p) + color_index;
            array[index++] = num_colors * ((s + 1) * points + p + 1) + color_index;
            array[index++] = num_colors * (s * points + p + 1) + color_index;
            if (random_colors) {
                color_index = rand() % num_colors;
            } else {
                color_index++;
                color_index %= num_colors;
            }
        }
    }
    return array;
}

float* Shapes::boxVertices(float height, float width, float depth,
                           const int num_colors)
{
    float* array = new float[3 * 8 * num_colors];
    float coords[3 * 8] = {
        -width / 2, height / 2, -depth / 2,
        -width / 2, height / 2, depth / 2,
        width / 2, height / 2, depth / 2,
        width / 2, height / 2, -depth / 2,
        -width / 2, -height / 2, -depth / 2,
        -width / 2, -height / 2, depth / 2,
        width / 2, -height / 2, depth / 2,
        width / 2, -height / 2, -depth / 2
    };
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < num_colors; j++) {
            for (int k = 0; k < 3; k++) {
                array[3 * num_colors * i + 3 * j + k] = coords[3 * i + k];
            }
        }
    }
    return array;
}

float* Shapes::boxColors(const int num_colors, float color_list[][3])
{
    float* array = new float[num_colors * 3 * 8];
    int index = 0;
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < num_colors; i++) {
            array[index++] = color_list[i][0];
            array[index++] = color_list[i][1];
            array[index++] = color_list[i][2];
        }
    }
    return array;
}

int* Shapes::boxIndices(const int num_colors, bool random_colors)
{
    srand(time(NULL));
    int* array = new int[12 * 3];
    int index = 0;
    int color_index = 0;
    if (random_colors) {
        color_index = rand() % num_colors;
    }
    array[index++] = 0 * num_colors + color_index;
    array[index++] = 1 * num_colors + color_index;
    array[index++] = 2 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 2 * num_colors + color_index;
    array[index++] = 3 * num_colors + color_index;
    array[index++] = 0 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 1 * num_colors + color_index;
    array[index++] = 5 * num_colors + color_index;
    array[index++] = 6 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 6 * num_colors + color_index;
    array[index++] = 2 * num_colors + color_index;
    array[index++] = 1 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 0 * num_colors + color_index;
    array[index++] = 4 * num_colors + color_index;
    array[index++] = 5 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 5 * num_colors + color_index;
    array[index++] = 1 * num_colors + color_index;
    array[index++] = 0 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 3 * num_colors + color_index;
    array[index++] = 2 * num_colors + color_index;
    array[index++] = 6 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 6 * num_colors + color_index;
    array[index++] = 7 * num_colors + color_index;
    array[index++] = 3 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 0 * num_colors + color_index;
    array[index++] = 3 * num_colors + color_index;
    array[index++] = 7 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 7 * num_colors + color_index;
    array[index++] = 4 * num_colors + color_index;
    array[index++] = 0 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 4 * num_colors + color_index;
    array[index++] = 7 * num_colors + color_index;
    array[index++] = 6 * num_colors + color_index;
    if (random_colors) {
        color_index = rand() % num_colors;
    } else {
        color_index++;
        color_index %= num_colors;
    }
    array[index++] = 6 * num_colors + color_index;
    array[index++] = 5 * num_colors + color_index;
    array[index++] = 4 * num_colors + color_index;
    return array;
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


Object * Shapes::createQuadHouses(SceneManager* sm)
{
    Object* houses = sm->createObject();
    // quad houses
    float vertices[] = {-4+6,-4, 4+6,   4+6,-4, 4+6,   4+6, 4, 4+6,  -4+6, 4, 4+6,  // front face
                        -4+6,-4,-4+6,  -4+6,-4, 4+6,  -4+6, 4, 4+6,  -4+6, 4,-4+6,  // left face
                        4+6,-4,-4+6,  -4+6,-4,-4+6,  -4+6, 4,-4+6,   4+6, 4,-4+6,  // back face
                        4+6,-4, 4+6,   4+6,-4,-4+6,   4+6, 4,-4+6,   4+6, 4, 4+6,  // right face
                        4+6, 4, 4+6,   4+6, 4,-4+6,  -4+6, 4,-4+6,  -4+6, 4, 4+6,  // top face
                        -4+6,-4, 4+6,  -4+6,-4,-4+6,   4+6,-4,-4+6,   4+6,-4, 4+6,  // bottom face

                        -20,-4,20,  20,-4,20,  20,-4,-20,  -20,-4,-20,              // ground floor

                        -4+6,4, 4+6,   4+6,4, 4+6,  0+6,8, 4+6,                     // the roof
                        4+6,4, 4+6,   4+6,4,-4+6,  0+6,8,-4+6,   0+6,8, 4+6,
                        -4+6,4, 4+6,   0+6,8, 4+6,  0+6,8,-4+6,  -4+6,4,-4+6,
                        4+6,4,-4+6,  -4+6,4,-4+6,  0+6,8,-4+6,

                        -4+6,-4, 4-6,   4+6,-4, 4-6,   4+6, 4, 4-6,  -4+6, 4, 4-6,  // front face
                        -4+6,-4,-4-6,  -4+6,-4, 4-6,  -4+6, 4, 4-6,  -4+6, 4,-4-6,  // left face
                        4+6,-4,-4-6,  -4+6,-4,-4-6,  -4+6, 4,-4-6,   4+6, 4,-4-6,  // back face
                        4+6,-4, 4-6,   4+6,-4,-4-6,   4+6, 4,-4-6,   4+6, 4, 4-6,  // right face
                        4+6, 4, 4-6,   4+6, 4,-4-6,  -4+6, 4,-4-6,  -4+6, 4, 4-6,  // top face
                        -4+6,-4, 4-6,  -4+6,-4,-4-6,   4+6,-4,-4-6,   4+6,-4, 4-6,  // bottom face

                        -4+6,4, 4-6,   4+6,4, 4-6,  0+6,8, 4-6,                     // the roof
                        4+6,4, 4-6,   4+6,4,-4-6,  0+6,8,-4-6,  0+6,8, 4-6,
                        -4+6,4, 4-6,   0+6,8, 4-6,  0+6,8,-4-6, -4+6,4,-4-6,
                        4+6,4,-4-6,  -4+6,4,-4-6,  0+6,8,-4-6,

                        -4-6,-4, 4-6,   4-6,-4, 4-6,   4-6, 4, 4-6,  -4-6, 4, 4-6,  // front face
                        -4-6,-4,-4-6,  -4-6,-4, 4-6,  -4-6, 4, 4-6,  -4-6, 4,-4-6,  // left face
                        4-6,-4,-4-6,  -4-6,-4,-4-6,  -4-6, 4,-4-6,   4-6, 4,-4-6,  // back face
                        4-6,-4, 4-6,   4-6,-4,-4-6,   4-6, 4,-4-6,   4-6, 4, 4-6,  // right face
                        4-6, 4, 4-6,   4-6, 4,-4-6,  -4-6, 4,-4-6,  -4-6, 4, 4-6,  // top face
                        -4-6,-4, 4-6,  -4-6,-4,-4-6,   4-6,-4,-4-6,   4-6,-4, 4-6,  // bottom face

                        -4-6,4, 4-6,   4-6,4, 4-6,  0-6,8, 4-6,                     // the roof
                        4-6,4, 4-6,   4-6,4,-4-6,  0-6,8,-4-6,  0-6,8, 4-6,
                        -4-6,4, 4-6,   0-6,8, 4-6,  0-6,8,-4-6, -4-6,4,-4-6,
                        4-6,4,-4-6,  -4-6,4,-4-6,  0-6,8,-4-6,


                        -4-6,-4, 4+6,   4-6,-4, 4+6,   4-6, 4, 4+6,  -4-6, 4, 4+6,  // front face
                        -4-6,-4,-4+6,  -4-6,-4, 4+6,  -4-6, 4, 4+6,  -4-6, 4,-4+6,  // left face
                        4-6,-4,-4+6,  -4-6,-4,-4+6,  -4-6, 4,-4+6,   4-6, 4,-4+6,  // back face
                        4-6,-4, 4+6,   4-6,-4,-4+6,   4-6, 4,-4+6,   4-6, 4, 4+6,  // right face
                        4-6, 4, 4+6,   4-6, 4,-4+6,  -4-6, 4,-4+6,  -4-6, 4, 4+6,  // top face
                        -4-6,-4, 4+6,  -4-6,-4,-4+6,   4-6,-4,-4+6,   4-6,-4, 4+6,  // bottom face

                        -4-6,4, 4+6,   4-6,4, 4+6,  0-6,8, 4+6,                     // the roof
                        4-6,4, 4+6,   4-6,4,-4+6,  0-6,8,-4+6,  0-6,8, 4+6,
                        -4-6,4, 4+6,   0-6,8, 4+6,  0-6,8,-4+6, -4-6,4,-4+6,
                        4-6,4,-4+6,  -4-6,4,-4+6,  0-6,8,-4+6,

    };

    float colors[] = {1,0,0, 1,1,0, 1,0,0, 1,0,1,
                      0,1,0, 0,1,0, 0,1,1, 0,1,0,
                      1,0,1, 1,0,0, 1,0,1, 1,0,0,
                      1,1,0, 0,1,0, 0,1,1, 0,1,0,
                      0,0,1, 1,0,1, 0,0,1, 0,1,1,
                      1,0,1, 0,1,1, 0,0,1, 0,0,1,

                      0,0,0, 0,1,0, 1,1,0, 1,0,0,  // ground floor

                      0,1,1, 0,0,1, 1,0,1,         // roof
                      1,0,0, 1,0,1, 1,0,0, 1,1,0,
                      1,1,0, 0,1,0, 1,1,0, 0,1,0,
                      0,1,1, 0,0,1, 1,0,1,

                      1,0,0, 1,1,0, 1,0,0, 1,0,1,
                      0,1,0, 0,1,0, 0,1,1, 0,1,0,
                      1,0,1, 1,0,0, 1,0,1, 1,0,0,
                      1,1,0, 0,1,0, 0,1,1, 0,1,0,
                      0,0,1, 1,0,1, 0,0,1, 0,1,1,
                      1,0,1, 0,1,1, 0,0,1, 0,0,1,

                      0,1,1, 0,0,1, 1,0,1,         // roof
                      1,0,0, 1,0,1, 1,0,0, 1,1,0,
                      1,1,0, 0,1,0, 1,1,0, 0,1,0,
                      0,1,1, 0,0,1, 1,0,1,

                      1,0,0, 1,1,0, 1,0,0, 1,0,1,
                      0,1,0, 0,1,0, 0,1,1, 0,1,0,
                      1,0,1, 1,0,0, 1,0,1, 1,0,0,
                      1,1,0, 0,1,0, 0,1,1, 0,1,0,
                      0,0,1, 1,0,1, 0,0,1, 0,1,1,
                      1,0,1, 0,1,1, 0,0,1, 0,0,1,

                      0,1,1, 0,0,1, 1,0,1,         // roof
                      1,0,0, 1,0,1, 1,0,0, 1,1,0,
                      1,1,0, 0,1,0, 1,1,0, 0,1,0,
                      0,1,1, 0,0,1, 1,0,1,

                      1,0,0, 1,1,0, 1,0,0, 1,0,1,
                      0,1,0, 0,1,0, 0,1,1, 0,1,0,
                      1,0,1, 1,0,0, 1,0,1, 1,0,0,
                      1,1,0, 0,1,0, 0,1,1, 0,1,0,
                      0,0,1, 1,0,1, 0,0,1, 0,1,1,
                      1,0,1, 0,1,1, 0,0,1, 0,0,1,

                      0,1,1, 0,0,1, 1,0,1,         // roof
                      1,0,0, 1,0,1, 1,0,0, 1,1,0,
                      1,1,0, 0,1,0, 1,1,0, 0,1,0,
                      0,1,1, 0,0,1, 1,0,1,
    };
    int indices[] = {0,2,3, 0,1,2,         // front face
                     4,6,7, 4,5,6,         // left face
                     8,10,11, 8,9,10,      // back face
                     12,14,15, 12,13,14,   // right face
                     16,18,19, 16,17,18,   // top face
                     20,22,23, 20,21,22,   // bottom face

                     24,26,27, 24,25,26,   // ground floor

                     28,29,30,             // roof
                     31,33,34, 31,32,33,
                     35,37,38, 35,36,37,
                     39,40,41,

                     0+42, 2+42, 3+42,  0+42, 1+42, 2+42,   // front face
                     4+42, 6+42, 7+42,  4+42, 5+42, 6+42,   // left face
                     8+42,10+42,11+42,  8+42, 9+42,10+42,   // back face
                     12+42,14+42,15+42, 12+42,13+42,14+42,  // right face
                     16+42,18+42,19+42, 16+42,17+42,18+42,  // top face
                     20+42,22+42,23+42, 20+42,21+42,22+42,  // bottom face

                     28+42-28+24,29+42-28+24,30+42-28+24,   // roof
                     31+42-28+24,33+42-28+24,34+42-28+24, 31+42-28+24,32+42-28+24,33+42-28+24,
                     35+42-28+24,37+42-28+24,38+42-28+24, 35+42-28+24,36+42-28+24,37+42-28+24,
                     39+42-28+24,40+42-28+24,41+42-28+24,

                     0+80,2+80,3+80, 0+80,1+80,2+80,        // front face
                     4+80,6+80,7+80, 4+80,5+80,6+80,        // left face
                     8+80,10+80,11+80, 8+80,9+80,10+80,     // back face
                     12+80,14+80,15+80, 12+80,13+80,14+80,  // right face
                     16+80,18+80,19+80, 16+80,17+80,18+80,  // top face
                     20+80,22+80,23+80, 20+80,21+80,22+80,  // bottom face

                     28+80-4,29+80-4,30+80-4,               // roof
                     31+80-4,33+80-4,34+80-4, 31+80-4,32+80-4,33+80-4,
                     35+80-4,37+80-4,38+80-4, 35+80-4,36+80-4,37+80-4,
                     39+80-4,40+80-4,41+80-4,

                     0+118,2+118,3+118, 0+118,1+118,2+118,        // front face
                     4+118,6+118,7+118, 4+118,5+118,6+118,        // left face
                     8+118,10+118,11+118, 8+118,9+118,10+118,     // back face
                     12+118,14+118,15+118, 12+118,13+118,14+118,  // right face
                     16+118,18+118,19+118, 16+118,17+118,18+118,  // top face
                     20+118,22+118,23+118, 20+118,21+118,22+118,  // bottom face

                     28+118-4,29+118-4,30+118-4,                  // roof
                     31+118-4,33+118-4,34+118-4, 31+118-4,32+118-4,33+118-4,
                     35+118-4,37+118-4,38+118-4, 35+118-4,36+118-4,37+118-4,
                     39+118-4,40+118-4,41+118-4,
    };

    int nVerts = 42 + (24 + 14) * 3;
    int nIndices = 60 + (36 + 18) * 3;

    setupObject(houses, nVerts, nIndices, vertices, colors, NULL,  indices);

    return houses;


}
