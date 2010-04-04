#ifndef __GLRenderContext_h__
#define __GLRenderContext_h__

#include "glew.h"
#include <QtOpenGL>

#include "RE330_global.h"
#include "RenderContext.h"
#include "VertexDeclaration.h"
#include "Light.h"
#include "Material.h"

namespace RE330
{
    /** This class implements the abstract base class RenderContext and
        it is the main interface to the low-level graphics OpenGL API.
        @remarks
        This class also instantiates the Singleton class to make sure there is
        only one instance at any time.
    */

    class RE330_EXPORT GLRenderContext : public RenderContext
    {
    public:
        // GLRenderContext();
        // ~GLRenderContext();
        void init();
        void setViewport(int width, int height);
        void beginFrame();
        void endFrame();
        void setModelViewMatrix(const Matrix4 &m);
        void setProjectionMatrix(const Matrix4 &m);
        /** This is the main method for rendering objects. Note that it uses
            OpenGL vertex arrays. See the OpenGL 2.0 book for details.
        */
        void render(Object *object);

        static GLRenderContext& getSingleton(void);
        static GLRenderContext* getSingletonPtr(void);

    public:
        /** This function needs to be called by the SceneManager once per
            frame, before all objects are rendered. The function assumes
            your light sources are stored in a std::list.

            NOTE: You also need to add this declaration to the base class
            RenderContext.h so that the SceneManager can call it.
        */
        void setLights(const std::list<Light*> &lightList);

    private:
        /** This function needs to be called before rendering each object.
         */
        void setMaterial(Material *material);

    };

}

#endif
