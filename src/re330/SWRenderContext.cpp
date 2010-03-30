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
    // if (frameCounter > 300) {
    //     std::cout << "resetting frame counter" << std::endl;
    //     frameCounter = 0;
    // }
    // if (frameCounter == 0)
    // {
    //     timer.restart();
    //     std::cout << "timing" << std::endl;
    // }

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
    // frameCounter++;
    // if (frameCounter == 300)
    // {
    //     std::cout << "final fps = " << 300.f/(timer.elapsed())*1000.f << std::endl;
    // }
    mswWidget->setPixmap(QPixmap::fromImage(*image));
    mswWidget->repaint();
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
        // only displays vertices
        projectVertices(p);
    }
    else //PART_2
    {
        // move points into a Vector4 for future matrix multiplication
        Vector4 verts[] = { Vector4(p[0]), Vector4(p[1]), Vector4(p[2])  };


        //setup for computing min/max values in for loop
        float minX, minY = numeric_limits<float>::max();;
        float maxX, maxY = 0.0f;

        // loop combined for efficiency
        // project to image space
        // normalize x,y,z values to w
        // find min/max
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

        //switch between linear and perspective interpolation
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

// called by rasterize instead of image->setPixel to allow for
// easy z-buffering
void SWRenderContext::setPixel(int x, int y, QRgb c, float z)
{
    // update pixel if it's closer to us
    if(z > zbuffer[x][y])
    {
        zbuffer[x][y] = z;
        image->setPixel(x, y, c);
    }
}

void SWRenderContext::projectVertices(float p[3][4])
{
    // init values to reduce memory alloc
    float x, y, w = 0;
    QRgb value = qRgb(255, 255, 255);

    for (int vertex = 0; vertex <= NUM_TRIANGLE_VERTS; vertex++)
    {
        Vector4 values(p[vertex]);

        // Use viewport*projection*modelview matrix to project to screen.
        values = total * values;

        // normalize x, y to w
        w = values[3];
        x = floor(values[0] / w);
        y = floor(values[1] / w);

        //set pixel on screen, no z-buffer
        image->setPixel(x, y, value);
    }
}

//common, simple method of rasterizing that iterates through all pixels in a box
void SWRenderContext::basicRasterize(Vector4 verts[3], float c[3][4],
                                     float minX, float maxX,
                                     float minY, float maxY)
{
    QRgb value = qRgb(255, 255, 255);

    // step through all pixels in bounding box
    for (int row = minY; row < maxY; row++)
    {
        float x, y, z = 0.0f;
        for (int col = minX; col < maxX; col++)
        {
            // need to do computations to center of pixel
            x = col + .5;
            y = row + .5;

            // compute barycentric coordinates for each pixel
            float baryCoord[3];
            findBaryCoord(verts, x, y, baryCoord);
            float alpha = baryCoord[0];
            float beta = baryCoord[1];
            float gamma = baryCoord[2];

            // determine if coordinate is in triangle
            if (coordInTri(verts, alpha, beta, gamma))
            {
                //reduce array calls and give nice names
                float a_z = verts[0][2];
                float b_z = verts[1][2];
                float c_z = verts[2][2];

                // linerally interpolate z value
                z = a_z + beta * (b_z - a_z) + gamma * (c_z - a_z);

                //find color values appropriately
                if(LINEAR)
                {
                    value = linearColor(c, alpha, beta, gamma);
                }
                else //perspective correct
                {
                    value = perspectiveColor(verts, c, alpha, beta, gamma);
                }
                // set necessary pixel
                setPixel(col, row, value, z);
            }
        }
    }
}

// uses a two-level hierarchy to reduce computation involved in basic version
// calls basic version on smalled boxes
void SWRenderContext::hierarchyRasterize( Vector4 verts[3], float c[3][4],
                                          float minX, float maxX,
                                          float minY, float maxY )
{
    // size of tiles
    int colStep = 15;
    int rowStep = 15;

    //convert max values to appropriate ints
    int iMaxX = ceil(maxX);
    int iMaxY = ceil(maxY);

    // test tiles left->right then bottom->top
    for (int row = minY; row < iMaxY; row+=rowStep)
    {
        // update y bounds
        int bottomY = row;

        // trim to max value to keep from going out-of-bounds
        int topY = std::min(iMaxY, bottomY + rowStep);

        for (int col = minX; col < iMaxX; col+=colStep)
        {
            int leftX = col;

            // trim to max value to keep from going out-of-bounds
            int rightX = std::min(iMaxX, leftX + colStep);

            // determine if we need to rasterize tile
            if( triVertInTile(verts, leftX, rightX, bottomY, topY) ||
                tileVertInTri(verts, leftX, rightX, bottomY, topY) ||
                edgesIntersect(verts, leftX, rightX, bottomY, topY) )
            {
                basicRasterize(verts, c, leftX, rightX, bottomY, topY);
            }
        }
    }
}

QRgb SWRenderContext::linearColor(float c[3][4],
                                  float alpha, float beta, float gamma)
{
    // linerally interpolate color values
    float color_1 = 255.f * (alpha * c[0][0] +
                           beta * (c[1][0] ) +
                           gamma * (c[2][0]));
    float color_2 = 255.f * (alpha * c[0][1] +
                           beta * (c[1][1]) +
                           gamma * (c[2][1]));
    float color_3 =  255.f * (alpha * c[0][2] +
                           beta * (c[1][2]) +
                           gamma * (c[2][2]));

    return qRgb(color_1, color_2, color_3);
}

QRgb SWRenderContext::perspectiveColor(Vector4 verts[3], float c[3][4],
                                       float alpha, float beta, float gamma)
{
    float a_w = verts[0][3];
    float b_w = verts[1][3];
    float c_w = verts[2][3];

    // interpolate appropriate to perspective
    float w_inverse = (alpha / a_w +
                       beta / b_w +
                       gamma / c_w);

    float color_1 = 255.f * (( alpha * c[0][0] / a_w +
                             beta * c[1][0] / b_w +
                             gamma * c[2][0] / c_w) /
                           w_inverse);

    float color_2 = 255.f * (( alpha * c[0][1] / a_w +
                             beta * c[1][1] / b_w +
                             gamma * c[2][1] / c_w) /
                           w_inverse );

    float color_3 = 255.f * (( alpha * c[0][2] / a_w +
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
            if (x >= leftX && x <= rightX &&
                y >= bottomY && y <= topY)
            {
                return true;
            }
        }
    return false;
}

// check each rectVertex for being in triangle
bool SWRenderContext::tileVertInTri(Vector4 verts[3],
                                    int leftX, int rightX,
                                    int bottomY, int topY)
{
    float baryCoord[3];

    // lower left vertex
    // compute barycentric coordinates for each pixel
    findBaryCoord(verts, leftX, bottomY, baryCoord);
    float alpha_ll = baryCoord[0];
    float beta_ll = baryCoord[1];
    float gamma_ll = baryCoord[2];

    // test if baryCoord are in triangle
    if (coordInTri(verts, alpha_ll, beta_ll, gamma_ll)) { return true; }

    // lower right vertex
    findBaryCoord(verts, rightX, bottomY, baryCoord);
    float alpha_lr = baryCoord[0];
    float beta_lr = baryCoord[1];
    float gamma_lr = baryCoord[2];
    if (coordInTri(verts, alpha_lr, beta_lr, gamma_lr)) { return true; }

    // upper left vertex
    findBaryCoord(verts, leftX, topY, baryCoord);
    float alpha_ul = baryCoord[0];
    float beta_ul = baryCoord[1];
    float gamma_ul = baryCoord[2];
    if (coordInTri(verts, alpha_ul, beta_ul, gamma_ul)) { return true; }

    // upper right vertex
    findBaryCoord(verts, rightX, topY, baryCoord);
    float alpha_ur = baryCoord[0];
    float beta_ur = baryCoord[1];
    float gamma_ur = baryCoord[2];
    if (coordInTri(verts, alpha_ur, beta_ur, gamma_ur)) { return true; }

    return false;
}

// check for edge intersections, edge by edge
bool SWRenderContext::edgesIntersect(Vector4 verts[3],
                                     float leftX, float rightX,
                                     float bottomY, float topY)
{
    // rect_1 = lower-left to lower-right
    // intersection rect_1 tri_ab
    if (segmentsIntersect(leftX, bottomY, rightX, bottomY,
                  verts[0][0], verts[0][1], verts[1][0], verts[1][1]))
    { return true; }

    // intersection rect_1 tri_bc
    if (segmentsIntersect(leftX, bottomY, rightX, bottomY,
                  verts[1][0], verts[1][1], verts[2][0], verts[2][1]))
    { return true; }

    // intersection rect_1 tri_ca
    if (segmentsIntersect(leftX, bottomY, rightX, bottomY,
                  verts[2][0], verts[2][1], verts[0][0], verts[0][1]))
    { return true; }


    //rect_2 = lower-right to upper-right
    // intersection rect_2 tri_ab
    if (segmentsIntersect(rightX, bottomY, rightX, topY,
                  verts[0][0], verts[0][1], verts[1][0], verts[1][1]))
    { return true; }

    // intersection rect_2 tri_bc
    if (segmentsIntersect(rightX, bottomY, rightX, topY,
                  verts[1][0], verts[1][1], verts[2][0], verts[2][1]))
    { return true; }

    // intersection rect_2 tri_ca
    if (segmentsIntersect(rightX, bottomY, rightX, topY,
                  verts[2][0], verts[2][1], verts[0][0], verts[0][1]))
    { return true; }


    //rect_3 = upper-right to upper-left
    // intersection rect_3 tri_ab
    if (segmentsIntersect(rightX, topY, leftX, topY,
                  verts[0][0], verts[0][1], verts[1][0], verts[1][1]))
    { return true; }

    // intersection rect_3 tri_bc
    if (segmentsIntersect(rightX, topY, leftX, topY,
                  verts[1][0], verts[1][1], verts[2][0], verts[2][1]))
    { return true; }

    // intersection rect_3 tri_ca
    if (segmentsIntersect(rightX, topY, leftX, topY,
                  verts[2][0], verts[2][1], verts[0][0], verts[0][1]))
    { return true; }


    //rect_4 = upper-left to lower-left
    // intersection rect_3 tri_ab
    if (segmentsIntersect(leftX, topY, leftX, bottomY,
                  verts[0][0], verts[0][1], verts[1][0], verts[1][1]))
    { return true; }

    // intersection rect_3 tri_bc
    if (segmentsIntersect(leftX, topY, leftX, bottomY,
                  verts[1][0], verts[1][1], verts[2][0], verts[2][1]))
    { return true; }

    // intersection rect_3 tri_ca
    if (segmentsIntersect(leftX, topY, leftX, bottomY,
                  verts[2][0], verts[2][1], verts[0][0], verts[0][1]))
    { return true; }

    return false;
}

// find baryCoord of a given x,y
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

// check if baryCoord are in triangle given by verts
bool SWRenderContext::coordInTri(Vector4 verts[3],
                                 float alpha, float beta, float gamma)
{
    // std- determine if coordinate is in triangle
    if ( (0.f < alpha && alpha < 1.f) &&
         (0.f < beta && beta < 1.f) &&
         (0.f < gamma && gamma < 1.f) )
    {
        return true;
    }

    // checks necessary for top-left case
    float a_x = verts[0][0];
    float a_y = verts[0][1];
    float b_x = verts[1][0];
    float b_y = verts[1][1];
    float c_x = verts[2][0];
    float c_y = verts[2][1];

    // go through in counter-clockwise order
    // if line is going down or horizontal, draw it
    if ((a_y - b_y <= 0.f) &&
        (gamma == 0.f) &&
        (0.f < alpha && alpha < 1.f) &&
        (0.f < beta && beta < 1.f))
    {
        return true;
    }

    // if line is going down or horizontal, draw it
    if ((b_y - c_y <= 0.f) &&
        (alpha == 0.f) &&
        (0.f < beta && beta < 1.f) &&
        (0.f < gamma && gamma < 1.f))
    {
        return true;
    }

    // if line is going down or horizontal, draw it
    if ((c_y - a_y <= 0.f) &&
        (beta == 0.f) &&
        (0.f < alpha && alpha < 1.f) &&
        (0.f < gamma && gamma < 1.f))
    {
        return true;
    }

    return false;
}

//find if the segment defined by (x1,y1)->(x2,y2) intersects (x3,y3)->(x4,y4)
bool SWRenderContext::segmentsIntersect(float x1, float y1, float x2, float y2,
                                        float x3, float y3, float x4, float y4)
{
    // calculates point of intersection using
    // http://mathworld.wolfram.com/Line-LineIntersection.html
    float denom = ((x1 - x2)*(y3 - y4) - (y1 - 2)*(x3 - x4));
    float x = ((x1 - x2)*(x3*y4 - x4*y3) - (x3 - x4)*(x1*y2 - y1*x2)) / denom;
    float y = ((y1 - y2)*(x3*y4 - x4*y3) - (y3 - y4)*(x1*y2 - y1*x2)) / denom;

    // if they don't intersect one of the coords will be invalid
    if (isnan(x) || isnan(y) || isinf(x) || isinf(y))
    {
        return false;
    }

    return true;
}
