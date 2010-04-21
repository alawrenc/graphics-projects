#include "SceneManager.h"
#include "RenderContext.h"
#include "TransformGroup.h"

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
    if(rootNode)
    {
        delete rootNode;
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

void SceneManager::setRootNode(Node *n) {
	rootNode = n;
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

        renderContext->setModelViewMatrix(Matrix4::IDENTITY);
        renderContext->setLights(mLightList);

		rootNode->draw(Matrix4(), *renderContext, *mCamera);

        renderContext->endFrame();
    }
}
