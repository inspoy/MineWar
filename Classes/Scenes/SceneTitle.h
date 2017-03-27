//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_SCENES_SCENETITLE_H_INCLUDED
#define ECHO_SCENES_SCENETITLE_H_INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/GameObject/GameScene.h"
#include "Scenes/LoginListener.h"

namespace Echo
{

    class SceneTitle : public GameScene
    {
    public:
        bool init();
        bool uninit();
        void initController();

    private:
        LoginListener* m_loginListener;
    };

}

#endif