//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Scenes/SceneLoadMap.h"
#include "Scenes/MapLoader.h"

namespace Echo
{

    bool SceneLoadMap::init()
    {
        if (!super::init()) return false;

        m_loader = NULL;

        m_sceneName = "SceneLoadMap";

        return true;
    }

    bool SceneLoadMap::uninit()
    {
        m_loader->uninit();
        CC_SAFE_DELETE(m_loader);

        return super::uninit();
    }

    void SceneLoadMap::initController()
    {
        m_hintNode = m_sceneLayer->getChildByName("Hint");
        ECHO_ASSERT(m_hintNode, "Cannot find the hint node.");

        m_loader = new MapLoader();
        m_loader->setOwner(this);
        m_loader->init("MapLoader");
    }

    cocos2d::Node* SceneLoadMap::getHintNode()
    {
        return m_hintNode;
    }

}