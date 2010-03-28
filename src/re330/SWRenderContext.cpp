#include "SWRenderContext.h"
#include "SWWidget.h"
using namespace RE330;

#include <math.h>
#include <limits>
#include <stdexcept>
using namespace std;

SWRenderContext* SWRenderContext::getSingletonPtr(void)
{
    return static_cast<SWRenderContext *>(ms_Singleton);
}

SWRenderContext& SWRenderContext::getSingleton(void)
{
    assert( ms_Singleton );
    return ( *(static_cast<SWRenderContext *>(ms_Singleton)) );
}

void SWRenderContext::init()
{
    modelview = Matrix4::IDENTITY;
    projection = Matrix4::IDENTITY;
    viewport = Matrix4::IDENTITY;
    total = Matrix4::IDENTITY;
}

void SWRenderContext::setViewport(int width, int height)
{
    // part 1.1
    // Compute viewport matrix based on width, height.
    viewport = Matrix4( width / 2.0f, 0, 0, width / 2.0f,
                        0, -height / 2.0f, 0, height / 2.0f,
                        0, 0, -.5, .5,
                        0, 0, 0, 1);
}

void SWRenderContext::beginFrame()
{
    image->fill(qRgb(0,0,0));
    int height = image->height();
    int width = image->width();
    zbuffer = new float*[width];
    for(int i = 0; i < width; i++)
    {
        *(zbuffer + i) = new float[height];
    }

}

void SWRenderContext::endFrame()
{
    mswWidget->setPixmap(QPixmap::fromImage(*image));
    mswWidget->repaint();
}

void SWRenderContext::setModelViewMatrix(const Matrix4 &m)
{
    // part 1.1
    // Set modelview matrix.
    modelview = m;
}

void SWRenderContext::setProjectionMatrix(const Matrix4 &m)
{
    //part 1.1
    // Set projection matrix.
    projection = m;
}

void SWRenderContext::render(Object *object)
{
    VertexData& vertexData = object->vertexData;
    VertexDeclaration& vertexDeclaration = vertexData.vertexDeclaration;
    VertexBufferBinding& vertexBufferBinding = vertexData.vertexBufferBinding;

    unsigned char *pPtr;	// Pointer to vertex position

    // Stride, i.e., number of bytes between vertex entries
    int pStr;

    // Size, i.e., number of vector elements per vertex position (typically 3)
    int pSze;

    unsigned char *nPtr;
    int nStr;
    int nSze;

    unsigned char *cPtr;
    int cStr;
    int cSze;

    pPtr = 0;	// Disable all arrays per default
    nPtr = 0;
    cPtr = 0;

    // Set up arrays
    for(int j = 0; j < vertexDeclaration.getElementCount(); j++)
    {
        const VertexElement *element = vertexDeclaration.getElement(j);

        const VertexBuffer& vertexBuffer =
            vertexBufferBinding.getBuffer(element->getBufferIndex());

        unsigned char* buf = vertexBuffer.getBuffer();

        int vertexStride = static_cast<int>(element->getStride());
        int vertexSize = static_cast<int>(element->getSize());
        int offset = element->getOffset();

        switch(element->getSemantic())
        {
        case VES_POSITION :
            pPtr = buf+offset;
            pStr = vertexStride;
            pSze = vertexSize;
            break;
        case VES_NORMAL :
            nPtr = buf+offset;
            nStr = vertexStride;
            nSze = 3;	// always
            break;
        case VES_DIFFUSE :
            cPtr = buf+offset;
            cStr = vertexStride;
            cSze = vertexSize;
            break;
        }
    }

    // Draw
    float p[3][4];	// Triangle vertex positions
    float c[3][4];	// Triangle vertex colors
    float n[3][3];	// Triangle vertex normals

    int *iPtr = vertexData.getIndexBuffer();

    for(int i=0; i < vertexData.getIndexCount(); i++)
    {
        // Local index of current triangle vertex
        int k = i%3;

        // Set default values
        p[k][0] = p[k][1] = p[k][2] = 0;
        p[k][3] = 1.f;
        c[k][0] = c[k][1] = c[k][2] = 1.f;
        c[k][3] = 0;
        n[k][0] = n[k][1] = 0;
        n[k][2] = 1;

        // Vertex position
        if(pPtr)
        {
            float *cur = (float*)(pPtr + pStr*iPtr[i]);
            for(int s=0; s<pSze; s++)
            {
                p[k][s] = *cur;
                cur++;
            }
        }

        // Vertex normal
        if(nPtr)
        {
            float *cur = (float*)(nPtr + nStr*iPtr[i]);
            for(int s=0; s<nSze; s++)
            {
                n[k][s] = *cur;
                cur++;
            }
        }

        // Vertex color
        if(cPtr)
        {
            float *cur = (float*)(cPtr + cStr*iPtr[i]);
            for(int s=0; s<cSze; s++)
            {
                c[k][s] = *cur;
                cur++;
            }
        }

        // Draw triangle
        if(i%3 == 2)
        {
            rasterizeTriangle(p, n, c);
        }
    }
}

void SWRenderContext::rasterizeTriangle(float p[3][4], float n[3][3], float c[3][4])
{
    // part 1.2
    // Implement triangle rasterization here.
    bool PART_1 = false;
    int NUM_TRIANGLE_VERTS = 3;
    QRgb value = qRgb(255, 255, 255);

    if(PART_1)
    {

        float x, y, w = 0;
        for (int vertex = 0; vertex <= NUM_TRIANGLE_VERTS; vertex++)
        {
            Vector4 values(p[vertex]);

            // Use viewport*projection*modelview matrix to project to screen.
            values = viewport * projection * (modelview * values);

            w = values[3];
            x = floor(values[0] / w);
            y = floor(values[1] / w);

            //set pixel on screen
            image->setPixel(x, y, value);
        }
    }
    else //PART_2
    {
        Vector4 verts[] = { Vector4(p[0]), Vector4(p[1]), Vector4(p[2])  };

        //setup for computing min/max values in for loop
        float minX, minY = numeric_limits<float>::max();;
        float maxX, maxY = 0.0f;

        // compute a bounding box around triangle
        for(int vertex = 0; vertex <= NUM_TRIANGLE_VERTS; vertex++)
        {
            float x, y, z, w = 0.0f;
            // convert vertices to pixel coordinates
            verts[vertex] = viewport * projection * (modelview * verts[vertex]);

            // normalize to w
            w = verts[vertex][3];
            x = floor(verts[vertex][0] / w);
            y = floor(verts[vertex][1] / w);
            z = verts[vertex][2] / w;

            verts[vertex] = Vector4(x, y, z, w);

            // update min/max values
            minX = std::min(verts[vertex][0], minX);
            maxX = std::max(verts[vertex][0], maxX);
            minY = std::min(verts[vertex][1], minY);
            maxY = std::max(verts[vertex][1], maxY);
        }

        // trim minX, minY to 0
        minY = max(0.0f, minY);
        minX = max(0.0f, minX);

        // trim maxX to image width (-1 needed since it's 0 based)
        maxX = min((float)image->width() - 1, maxX);

        // trim maxY to image height
        maxY = min((float)image->height() - 1, maxY);

        // values for finding barycentric coordinates
        float a_x = verts[0][0];
        float a_y = verts[0][1];
        float a_z = verts[0][2];
        float b_x = verts[1][0];
        float b_y = verts[1][1];
        float b_z = verts[1][2];
        float c_x = verts[2][0];
        float c_y = verts[2][1];
        float c_z = verts[2][2];
        float detT = (c_x - a_x) * (b_y - a_y) - (b_x -a_x) * (c_y - a_y);

        // step through all pixels in bounding box
        for (int col = minY; col <= maxY; col++)
        {
            float alpha, beta, gamma = 0.0f;
            float x, y, z = 0.0f;
            for (int row = minX; row <= maxX; row++)
            {
                x = col + .5;
                y = row + .5;
                // compute barycentric coordinates for center of each pixel
                beta = ((a_y - c_y) * (x - a_x) +
                        (c_x - a_x) * (y - a_y)) / detT;
                gamma = ((b_y - a_y) * (x - a_x) +
                         (a_x - b_x) * (y - a_y)) / detT;
                alpha = 1 - beta - gamma;

                // determine if coordinate is in triangle
                if ( (0 < alpha && alpha < 1) &&
                     (0 < beta && beta < 1) &&
                     (0 < gamma && gamma < 1) )
                {
                    // for(int i = 0; i<3; i++)
                    // {
                    //     for (int j = 0; j<4; j++)
                    //     {
                    //         std::cout <<" "<< c[i][j] << std::endl;
                    //     }
                    // }
                    // std::cout << "" << std::endl;
                    //image->setPixel(row, col, value);
                    //TO-DO: linerally interpolate z value
                    float color_1 = 255*(c[0][0] +
                                     beta * (c[1][0] - c[0][0]) +
                                     gamma * (c[2][0]) - c[0][0]);
                    float color_2 = 255*(c[0][1] +
                                     beta * (c[1][1] - c[0][1]) +
                                     gamma * (c[2][1]) - c[0][1]);
                    float color_3 = 255*(c[0][2] +
                                     beta * (c[1][2] - c[0][2]) +
                                     gamma * (c[2][2]) - c[0][2]);

                    z = a_z + beta * (b_z - a_z) + gamma * (c_z - a_z);
                    value = qRgb(color_1, color_2, color_3);
                    setPixel(row, col, value, z);
                }
            }
        }

    }
}

void SWRenderContext::setWidget(SWWidget *swWidget)
{
    mswWidget = swWidget;
    image = mswWidget->getImage();
}

void SWRenderContext::setPixel(int x, int y, QRgb c, float z)
{
    if(zbuffer[x][y])
    {
        //std::cout << zbuffer[x][y] << std::endl;
        if(z > zbuffer[x][y])
        {
            zbuffer[x][y] = z;
            image->setPixel(x, y, c);
        }
    }
    else
    {
        zbuffer[x][y] = z;
        image->setPixel(x, y, c);
    }
}
