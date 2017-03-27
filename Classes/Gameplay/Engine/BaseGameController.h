//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_BASEGAMECONTROLLER_H__INCLUDED
#define ECHO_GAMEPLAY_BASEGAMECONTROLLER_H__INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/UIController.h"

namespace Echo
{
    class GameScene;

    class BaseGameController
    {
    public:
        typedef BaseGameController super;

        BaseGameController();
        virtual bool init(String controllerName);
        virtual bool uninit();
        virtual void update(f32 dt) = 0;

        void setOwner(GameScene* pOwner);

        String getSceneName();
        String getControllerName();
        UIController* getUIController();

    protected:
        String m_controllerName;

        GameScene* m_pCurrentScene;
        UIController* m_pUiController;
    };
}

#endif