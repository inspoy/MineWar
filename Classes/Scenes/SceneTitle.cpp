//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Scenes/SceneTitle.h"

namespace Echo
{

    bool SceneTitle::init()
    {
        if (!super::init()) return false;

        m_loginListener = NULL;

        m_sceneName = "SceneTitle";

        return true;
    }

    bool SceneTitle::uninit()
    {
        m_loginListener->uninit();
        CC_SAFE_DELETE(m_loginListener);

        return super::uninit();
    }

    void SceneTitle::initController()
    {
        m_loginListener = new LoginListener();
        m_loginListener->setOwner(this);
        m_loginListener->init("LoginListener");
    }

}
