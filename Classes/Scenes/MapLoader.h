//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/BaseGameController.h"

namespace Echo
{

    class SceneLoadMap;

    class MapLoader : public BaseGameController
    {
    public:
        MapLoader();
        bool init(String controllerName);
        bool uninit();
        void update(f32 dt);

        // =====================

    private:
        f32 m_timer;

        static const f32 LOAD_DELAY;
    };

}