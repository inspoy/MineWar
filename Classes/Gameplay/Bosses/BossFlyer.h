//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "Precompiled.h"
#include "Gameplay/BaseBossController.h"

namespace Echo
{

    class BossFlyer : public BaseBossController
    {
    public:
        BossFlyer();
        ~BossFlyer();
        bool init(Boss* boss);
        bool uninit();
        void update(f32 dt);
        void collide(f32 dt);

    private:
        f32 m_bossGodTime;
        f32 m_heroGodTime;
    };

}