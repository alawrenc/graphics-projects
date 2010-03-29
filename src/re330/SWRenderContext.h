#ifndef __SWRenderContext_h__
#define __SWRenderContext_h__


#include "RE330_global.h"
#include "RenderContext.h"
#include "VertexDeclaration.h"
#include "Matrix4.h"

namespace RE330 {

    /** This class implements the abstract base class RenderContext and
        it is the main interface to the low-level graphics API.
    @remarks
        This class also instantiates the Singleton class to make sure there is
        only one instance at any time.
    */

    class SWWidget;

    class RE330_EXPORT SWRenderContext : public RenderContext
    {
    public:
        void init();
        void setViewport(int width, int height);
        void beginFrame();
        void endFrame();
        void setModelViewMatrix(const Matrix4 &m);
        void setProjectionMatrix(const Matrix4 &m);
        /** This is the main method for rendering objects. Note that it uses
            OpenSW vertex arrays. See the OpenSW 2.0 book for details.
        */
        void render(Object *object);

        /** Set the widget that will receive rendering output.
        */
        void setWidget(SWWidget *swWidget);

        static SWRenderContext& getSingleton(void);
        static SWRenderContext* getSingletonPtr(void);

    private:
        SWWidget *mswWidget;
        QImage *image;

        static const int NUM_TRIANGLE_VERTS = 3;
        static const bool PART_1 = false;
        static const bool LINEAR = false;
        static const bool BASIC = true;

        static const int TILE_DIVISIONS = 3;

        Matrix4 projection;
        Matrix4 modelview;
        Matrix4 viewport;
        Matrix4 total;
        float ** zbuffer;

        void rasterizeTriangle(float p[3][4], float n[3][3], float c[3][4]);
        void setPixel(int x, int y, QRgb c, float z);
        void projectVertices(float p[3][4]);
        void basicRasterize(Vector4 v[3], float c[3][4],
                            float minX, float maxX,
                            float minY, float maxY);
        void hierarchyRasterize(Vector4 verts[3], float c[3][4],
                                float minX, float maxX,
                                float minY, float maxY);
        QRgb linearColor(float c[3][4], float beta, float gamma);
        QRgb perspectiveColor(Vector4 verts[3], float c[3][4],
                              float alpha, float beta, float gamma);
        bool triVertInTile(Vector4 verts[3],
                           int leftX, int rightX,
                           int bottomY, int topY);
        bool tileVertInTri(Vector4 verts[3],
                           int leftX, int rightX,
                           int bottomY, int topY);
        bool edgesIntersect();
        float* findBaryCoord(Vector4 verts[3], float x, float y);
    };

}

#endif
