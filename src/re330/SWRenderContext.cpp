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

    //update total matrix
    total = viewport * projection * modelview;
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
    int width = image->width();
}

void SWRenderContext::setModelViewMatrix(const Matrix4 &m)
{
    // part 1.1
    // Set modelview matrix.
    modelview = m;

    //update total matrix
    total = viewport * projection * modelview;
}

void SWRenderContext::setProjectionMatrix(const Matrix4 &m)
{
    //part 1.1
    // Set projection matrix.
    projection = m;

    //update total matrix
    total = viewport * projection * modelview;
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
    if(PART_1)
    {
        projectVertices(p);
    }
    else //PART_2
    {
        Vector4 verts[] = { Vector4(p[0]), Vector4(p[1]), Vector4(p[2])  };

        for(int vertex = 0; vertex <= NUM_TRIANGLE_VERTS; vertex++)
        {
            // convert vertices to pixel coordinates
            verts[vertex] = total * verts[vertex];

            float x, y, z, w = 0.0f;
            // normalize to w
            // does this need to happen before viewport transform?
            w = verts[vertex][3];
            x = floor(verts[vertex][0] / w);
            y = floor(verts[vertex][1] / w);
            z = verts[vertex][2] / w;

            verts[vertex] = Vector4(x, y, z, w);
        }

        //setup for computing min/max values in for loop
        float minX, minY = numeric_limits<float>::max();;
        float maxX, maxY = 0.0f;

        // compute a bounding box around triangle
        for(int vertex = 0; vertex <= NUM_TRIANGLE_VERTS; vertex++)
        {
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
        if (BASIC)
        {
            basicRasterize(verts, c, minX, maxX, minY, maxY);
        }
        else
        {
            hierarchyRasterize(verts, c, minX, maxX, minY, maxY);
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
    if(zbuffer[x][y] == -1.0f)
    {
        zbuffer[x][y] = z;
        image->setPixel(x, y, c);
    }
    else
    {
        if(z > zbuffer[x][y])
        {
            zbuffer[x][y] = z;
            image->setPixel(x, y, c);
        }
    }
}

void SWRenderContext::projectVertices(float p[3][4])
{
    float x, y, w = 0;
    QRgb value = qRgb(255, 255, 255);
    for (int vertex = 0; vertex <= NUM_TRIANGLE_VERTS; vertex++)
    {
        Vector4 values(p[vertex]);

        // Use viewport*projection*modelview matrix to project to screen.
        values = total * values;

        w = values[3];
        x = floor(values[0] / w);
        y = floor(values[1] / w);

        //set pixel on screen
        image->setPixel(x, y, value);
    }
}

void SWRenderContext::basicRasterize(Vector4 verts[3], float c[3][4],
                                     float minX, float maxX,
                                     float minY, float maxY)
{
    QRgb value = qRgb(255, 255, 255);

    // step through all pixels in bounding box
    for (int col = minX; col < maxX; col++)
    {
        float x, y, z = 0.0f;
        for (int row = minY; row < maxY; row++)
        {
            x = col + .5;
            y = row + .5;
            // compute barycentric coordinates for each pixel
            float baryCoord[3];
            findBaryCoord(verts, x, y, baryCoord);
            float alpha = baryCoord[0];
            float beta = baryCoord[1];
            float gamma = baryCoord[2];

            // determine if coordinate is in triangle
            // TODO: determine if left/top and allow
            if ( (0 < alpha && alpha < 1) &&
                 (0 < beta && beta < 1) &&
                 (0 < gamma && gamma < 1) )
            {
                float a_z = verts[0][2];
                float b_z = verts[1][2];
                float c_z = verts[2][2];

                // linerally interpolate z value
                z = a_z + beta * (b_z - a_z) + gamma * (c_z - a_z);

                if(LINEAR)
                {
                    value = linearColor(c, alpha, beta, gamma);
                }
                else //perspective correct
                {
                    value = perspectiveColor(verts, c, alpha, beta, gamma);
                }
                setPixel(col, row, value, z);
            }
        }
    }
}

void SWRenderContext::hierarchyRasterize( Vector4 verts[3], float c[3][4],
                                          float minX, float maxX,
                                          float minY, float maxY )
{
    int colStep = (maxX - minX) / TILE_DIVISIONS;
    int rowStep = (maxY - minY) / TILE_DIVISIONS;

    for (int boxCol = 0; boxCol < TILE_DIVISIONS; boxCol++)
    {
        int leftX = boxCol * colStep;
        int rightX = leftX + colStep;

        for (int boxRow = 0; boxRow < TILE_DIVISIONS; boxRow++)
        {
            int bottomY = boxRow * rowStep;
            int topY = bottomY + rowStep;
            if( triVertInTile(verts, leftX, rightX, bottomY, topY) ||
                tileVertInTri(verts, leftX, rightX, bottomY, topY) ||
                edgesIntersect() )
            {
                basicRasterize(verts, c, leftX, rightX, bottomY, topY);
            }
        }
    }
// Check if any of the triangle's vertices are inside the rectangle
//     (e.g., compare the (x,y) coordinates with the min/max (x,y) coordinates of the rectangle)
// Check if any of the rectangle's vertices are inside the triangle
//     (e.g., use barycentric coordinates)
//  Check if any of the triangle's edges intersects any of the rectangle's edges.
}

QRgb SWRenderContext::linearColor(float c[3][4],
                                  float alpha,float beta, float gamma)
{
    // linerally interpolate color values
    float color_1 = 255 * (alpha * c[0][0] +
                           beta * (c[1][0] - c[0][0]) +
                           gamma * (c[2][0]) - c[0][0]);
    float color_2 = 255 * (alpha * c[0][1] +
                           beta * (c[1][1] - c[0][1]) +
                           gamma * (c[2][1]) - c[0][1]);
    float color_3 = 255 * (alpha * c[0][2] +
                           beta * (c[1][2] - c[0][2]) +
                           gamma * (c[2][2]) - c[0][2]);

    return qRgb(color_1, color_2, color_3);
}

QRgb SWRenderContext::perspectiveColor(Vector4 verts[3], float c[3][4],
                                       float alpha, float beta, float gamma)
{
    float a_w = verts[0][3];
    float b_w = verts[1][3];
    float c_w = verts[2][3];

    float w_inverse = (alpha / a_w +
                       beta / b_w +
                       gamma / c_w);

    float color_1 = 255 * (( alpha * c[0][0] / a_w +
                             beta * c[1][0] / b_w +
                             gamma * c[2][0] / c_w) /
                           w_inverse);

    float color_2 = 255 * (( alpha * c[0][1] / a_w +
                             beta * c[1][1] / b_w +
                             gamma * c[2][1] / c_w) /
                           w_inverse );

    float color_3 = 255 * (( alpha * c[0][2] / a_w +
                             beta * c[1][2] / b_w +
                             gamma * c[2][2] / c_w) /
                           w_inverse );

    return qRgb(color_1, color_2, color_3);
}

bool SWRenderContext::triVertInTile(Vector4 verts[3],
                                    int leftX, int rightX,
                                    int bottomY, int topY)
{
    // - Check if any of the triangle's vertices are inside the rectangle
    //     (e.g., compare the (x,y) coordinates with the min/max (x,y)
    //         coordinates of the rectangle)
        for(int v = 0; v < NUM_TRIANGLE_VERTS; v++)
        {
            float x = verts[v][0];
            float y = verts[v][1];
            if (x > leftX && x < rightX &&
                y > bottomY && y < topY)
            {
                return true;
            }
        }
    return false;
}

bool SWRenderContext::tileVertInTri(Vector4 verts[3],
                                    int leftX, int rightX,
                                    int bottomY, int topY)
{
    // values for finding barycentric coordinates
    float a_x = verts[0][0];
    float a_y = verts[0][1];
    float b_x = verts[1][0];
    float b_y = verts[1][1];
    float c_x = verts[2][0];
    float c_y = verts[2][1];

    float detT = (c_x - a_x) * (b_y - a_y) - (b_x - a_x) * (c_y - a_y);

    float alpha, beta, gamma = 0.0f;

    // compute barycentric coordinates for lower left vertex
    float b_bottomY = (c_x - a_x) * (bottomY - a_y);
    float b_leftX = (a_y - c_y) * (leftX - a_x);
    beta = (b_leftX + b_bottomY) / detT;

    float g_bottomY = (a_x - b_x) * (bottomY - a_y);
    float g_leftX = (b_y - a_y) * (leftX - a_x);
    gamma = (g_leftX + g_bottomY) / detT;
    alpha = 1 - beta - gamma;

    // determine if coordinate is in triangle
    if ( (0 < alpha && alpha < 1) &&
         (0 < beta && beta < 1) &&
         (0 < gamma && gamma < 1) )
    {
        return true;
    }

    // compute barycentric coordinates for lower right vertex
    float b_rightX = (a_y - c_y) * (rightX - a_x);
    beta = (b_rightX + b_bottomY) / detT;

    float g_rightX = (b_y - a_y) * (rightX - a_x);
    gamma = (g_rightX + g_bottomY) / detT;
    alpha = 1 - beta - gamma;

    // determine if coordinate is in triangle
    if ( (0 < alpha && alpha < 1) &&
         (0 < beta && beta < 1) &&
         (0 < gamma && gamma < 1) )
    {
        return true;
    }

    //compute barycentric coordinates for upper-left vertex
    float b_topY =  (c_x - a_x) * (topY - a_y);
    float g_topY = (a_x - b_x) * (topY - a_y);
    beta = (b_leftX + b_topY) / detT;
    gamma = (g_leftX + g_topY) / detT;
    alpha = 1 - beta - gamma;

    // determine if coordinate is in triangle
    if ( (0 < alpha && alpha < 1) &&
         (0 < beta && beta < 1) &&
         (0 < gamma && gamma < 1) )
    {
        return true;
    }
    return false;
}

bool SWRenderContext::edgesIntersect()
{
    return false;
}

void SWRenderContext::findBaryCoord(Vector4 verts[3],
                                      float x, float y,
                                      float coord[3])
{
    float a_x = verts[0][0];
    float a_y = verts[0][1];
    float b_x = verts[1][0];
    float b_y = verts[1][1];
    float c_x = verts[2][0];
    float c_y = verts[2][1];
    float detT = (c_x - a_x) * (b_y - a_y) - (b_x - a_x) * (c_y - a_y);

    float alpha, beta, gamma = 0.0f;
    beta = ((a_y - c_y) * (x - a_x) +
            (c_x - a_x) * (y - a_y)) / detT;
    gamma = ((b_y - a_y) * (x - a_x) +
             (a_x - b_x) * (y - a_y)) / detT;
    alpha = 1 - beta - gamma;

    coord[0] = alpha;
    coord[1] = beta;
    coord[2] = gamma;
}
