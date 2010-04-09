#include "GLRenderContext.h"


using namespace RE330;

// GLRenderContext::GLRenderContext()
// {
//     init();
// }

// GLRenderContext::~GLRenderContext()
// {
// }

GLRenderContext* GLRenderContext::getSingletonPtr(void)
{
    return static_cast<GLRenderContext *>(ms_Singleton);
}
GLRenderContext& GLRenderContext::getSingleton(void)
{
    assert( ms_Singleton );
    return ( *(static_cast<GLRenderContext *>(ms_Singleton)) );
}

void GLRenderContext::init()
{
    assert(glewInit() == GLEW_OK);
    assert(GL_VERSION_2_0);

    glShadeModel(GL_SMOOTH);    // turns on shading
    glEnable(GL_DEPTH_TEST);    // activates depth buffer for hidden surface removal
    glEnable(GL_CULL_FACE);     // makes faces invisible
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    // clears color and depth buffer

    glEnable(GL_NORMALIZE);
    glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
}

void GLRenderContext::setViewport(int width, int height)
{
    glViewport(0,0, width, height);    // sets size of area of window where graphics are displayed
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderContext::beginFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0,1.0,1.0,1.0);
}

void GLRenderContext::endFrame()
{
    glFlush();    // forces previously issued OpenGL commands to begin execution
}

void GLRenderContext::setModelViewMatrix(const Matrix4 &m)
{
    glMatrixMode(GL_MODELVIEW);    // selects model view matrix (used to describe affine transformations)
    glLoadMatrixf(m.transpose().getElementPointer());    // sets model view matrix to transpose of m
}

void GLRenderContext::setProjectionMatrix(const Matrix4 &m)
{
    glMatrixMode(GL_PROJECTION);    // selects projection matrix (used to describe orthographic and
                                    // perspective transformations)
    glLoadMatrixf(m.transpose().getElementPointer());    // sets projection matrix to transpose of m
}

void GLRenderContext::render(Object *object)
{
    setMaterial(object->getMaterial());
    VertexData& vertexData = object->vertexData;
    VertexDeclaration& vertexDeclaration = vertexData.vertexDeclaration;
    VertexBufferBinding& vertexBufferBinding = vertexData.vertexBufferBinding;

    // The basic way to draw triangles in OpenGL is to use glBegin(GL_TRIANGLES),
    // then specify vertices, colors, etc. using glVertex(), glColor(), etc., and finish
    // with glEnd(). However, we are using a more advanced technique here that is based
    // on so-called vertex arrays. Vertex arrays are more efficient because they need
    // fewer function calls to OpenGL. Read more about vertex arrays in Chapter 2 of the
    // OpenGL book.

    // Set up vertex arrays
    for(int j=0; j<vertexDeclaration.getElementCount(); j++)
    {
        const VertexElement *element = vertexDeclaration.getElement(j);

        const VertexBuffer& vertexBuffer = vertexBufferBinding.getBuffer(element->getBufferIndex());
        unsigned char* buf = vertexBuffer.getBuffer();

        GLsizei vertexStride = static_cast<GLsizei>(element->getStride());
        GLint vertexSize = static_cast<GLint>(element->getSize());
        int offset = element->getOffset();

        switch(element->getSemantic())
        {
        case VES_POSITION :
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(vertexSize, GL_FLOAT, vertexStride, buf+offset);
            break;
        case VES_NORMAL :
            glEnableClientState(GL_NORMAL_ARRAY);
            glNormalPointer(GL_FLOAT, vertexStride, buf+offset);
            break;
        case VES_DIFFUSE :
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(vertexSize, GL_FLOAT, vertexStride, buf+offset);
            glEnable(GL_COLOR_MATERIAL);
            break;
        case VES_TEXTURE_COORDINATES :
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glTexCoordPointer(vertexSize, GL_FLOAT, vertexStride, buf+offset);
            break;
        }
    }

    // Draw
    glDrawElements(GL_TRIANGLES, vertexData.getIndexCount(), GL_UNSIGNED_INT, vertexData.getIndexBuffer());

    // Disable the arrays we used
    for(int j=0; j<vertexDeclaration.getElementCount(); j++)
    {
        const VertexElement *element = vertexDeclaration.getElement(j);

        switch(element->getSemantic())
        {
        case VES_POSITION :
            glDisableClientState(GL_VERTEX_ARRAY);
            break;
        case VES_NORMAL :
            glDisableClientState(GL_NORMAL_ARRAY);
            break;
        case VES_DIFFUSE :
            glDisableClientState(GL_COLOR_ARRAY);
            break;
        case VES_TEXTURE_COORDINATES :
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            break;
        }
    }

    assert(glGetError()==GL_NO_ERROR);
}

void GLRenderContext::setLights(const std::list<Light*> &lightList)
{
    GLint lightIndex[] = {GL_LIGHT0, GL_LIGHT1, GL_LIGHT2, GL_LIGHT3, GL_LIGHT4, GL_LIGHT5, GL_LIGHT6, GL_LIGHT7};

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    std::list<Light*>::const_iterator iter;

    if(lightList.begin()!=lightList.end())
    {
        // Lighting
        glEnable(GL_LIGHTING);

        int i=0;
        for (iter=lightList.begin(); iter!=lightList.end() && i<8; iter++, i++)
        {
            Light *l = (*iter);

            glEnable(lightIndex[i]);

            if(l->getType() == Light::DIRECTIONAL)
            {
                float direction[4];
                direction[0] = l->getDirection()[0];
                direction[1] = l->getDirection()[1];
                direction[2] = l->getDirection()[2];
                direction[3] = 0.f;
                glLightfv(lightIndex[i], GL_POSITION, direction);
            }
            if(l->getType() == Light::POINT || l->getType() == Light::SPOT)
            {
                float position[4];
                position[0] = l->getPosition()[0];
                position[1] = l->getPosition()[1];
                position[2] = l->getPosition()[2];
                position[3] = 1.f;
                glLightfv(lightIndex[i], GL_POSITION, position);
            }
            if(l->getType() == Light::SPOT)
            {
                float spotDirection[3];
                spotDirection[0] = l->getSpotDirection()[0];
                spotDirection[1] = l->getSpotDirection()[1];
                spotDirection[2] = l->getSpotDirection()[2];
                glLightfv(lightIndex[i], GL_SPOT_DIRECTION, spotDirection);
                glLightf(lightIndex[i], GL_SPOT_EXPONENT, l->getSpotExponent());
                glLightf(lightIndex[i], GL_SPOT_CUTOFF, l->getSpotCutoff());
            }

            float diffuse[4];
            diffuse[0] = l->getDiffuseColor()[0];
            diffuse[1] = l->getDiffuseColor()[1];
            diffuse[2] = l->getDiffuseColor()[2];
            diffuse[3] = 1.f;
            glLightfv(lightIndex[i], GL_DIFFUSE, diffuse);

            float ambient[4];
            ambient[0] = l->getAmbientColor()[0];
            ambient[1] = l->getAmbientColor()[1];
            ambient[2] = l->getAmbientColor()[2];
            ambient[3] = 0;
            glLightfv(lightIndex[i], GL_AMBIENT, ambient);

            float specular[4];
            specular[0] = l->getSpecularColor()[0];
            specular[1] = l->getSpecularColor()[1];
            specular[2] = l->getSpecularColor()[2];
            specular[3] = 0;
            glLightfv(lightIndex[i], GL_SPECULAR, specular);
        }
    }
}

void GLRenderContext::setMaterial(Material *m)
{
    if(m!=0)
    {

        float diffuse[4];
        diffuse[0] = m->getDiffuse()[0];
        diffuse[1] = m->getDiffuse()[1];
        diffuse[2] = m->getDiffuse()[2];
        diffuse[3] = 1.f;
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

        float ambient[4];
        ambient[0] = m->getAmbient()[0];
        ambient[1] = m->getAmbient()[1];
        ambient[2] = m->getAmbient()[2];
        ambient[3] = 1.f;
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

        float specular[4];
        specular[0] = m->getSpecular()[0];
        specular[1] = m->getSpecular()[1];
        specular[2] = m->getSpecular()[2];
        specular[3] = 1.f;
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m->getShininess());

        Texture *tex = m->getTexture();
        if(tex!=0)
        {
            glEnable(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
            glBindTexture(GL_TEXTURE_2D, tex->getId());
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }

        if(m->getShader()!=0)
        {
            m->getShader()->use();
        }
    }
}
