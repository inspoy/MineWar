//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Scenes/MapLoader.h"
#include "Scenes/SceneLoadMap.h"
#include "Gameplay/Engine/GameLogic.h"

namespace Echo
{

    const f32 MapLoader::LOAD_DELAY = 1.5f;

    MapLoader::MapLoader()
        : m_timer(0.0f)
    {}

    bool MapLoader::init(String controllerName)
    {
        super::init(controllerName);
        m_timer = LOAD_DELAY;
        return true;
    }

    bool MapLoader::uninit()
    {
        return true;
    }

    void MapLoader::update(f32 dt)
    {
        if (m_timer > 0.0f)
        {
            m_timer -= dt;
        }
        else
        {
            GameLogic::getInstance()->switchScene("SceneGameMap");
        }
    }

}