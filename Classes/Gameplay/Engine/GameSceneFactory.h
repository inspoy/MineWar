//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_GAMESCENEFACTORY_H__INCLUDED
#define ECHO_GAMEPLAY_GAMESCENEFACTORY_H__INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"

namespace Echo
{

    class GameScene;

    class GameSceneFactory
    {
    public:
        ECHO_DECLARE_SINGLETON(GameSceneFactory);

    public:
        GameScene* createScene(String sceneName);

    private:
        static const u32 UI_TAG;
    };

}

#endif