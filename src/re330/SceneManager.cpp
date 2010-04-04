#include "SceneManager.h"
#include "RenderContext.h"

using namespace RE330;

SceneManager::SceneManager()
    : mCamera(0)
{
}

SceneManager::~SceneManager()
{
    if(mCamera)
    {
        delete mCamera;
    }
    while(mObjectList.size() > 0)
    {
        Object *o = mObjectList.front();
        mObjectList.pop_front();
        delete o;
    }
    while(mLightList.size() > 0)
    {
        Light *l = mLightList.front();
        mLightList.pop_front();
        delete l;
    }
}

Object* SceneManager::createObject()
{
    Object *o = new Object();
    mObjectList.push_back(o);

    return o;
}

Light* SceneManager::createLight()
{
    if (mLightList.size() >=8)
    {
        std::cout << "tried to add too many light sources!" << std::endl;
        throw;
    }
    Light *l = new Light();
    mLightList.push_back(l);

    return l;
}

Camera* SceneManager::createCamera()
{
    mCamera = new Camera();
    return mCamera;
}

void SceneManager::renderScene()
{
    RenderContext* renderContext = RenderContext::getSingletonPtr();

    if(mCamera!=0)
    {
        renderContext->beginFrame();

        renderContext->setProjectionMatrix(mCamera->getProjectionMatrix());
        Matrix4 v = mCamera->getViewMatrix();

        renderContext->setModelViewMatrix(Matrix4::IDENTITY);
        renderContext->setLights(mLightList);

        // Iterate through list of objects
        std::list<Object *>::const_iterator iter;
        for (iter=mObjectList.begin(); iter!=mObjectList.end(); iter++)
        {
            Object *o = (*iter);
            Matrix4 m = o->getTransformation();

            renderContext->setModelViewMatrix(v*m);
            renderContext->render(o);
        }

        renderContext->endFrame();
    }
}
