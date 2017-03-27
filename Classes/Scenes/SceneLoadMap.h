//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/GameObject/GameScene.h"

namespace Echo
{

    class MapLoader;

    class SceneLoadMap : public GameScene
    {
    public:
        bool init();
        bool uninit();
        void initController();

        cocos2d::Node* getHintNode();
    private:
        MapLoader* m_loader;
        cocos2d::Node* m_hintNode;
    };

}