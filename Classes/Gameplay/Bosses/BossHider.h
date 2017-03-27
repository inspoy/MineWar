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

    class BossHider : public BaseBossController
    {
    public:
        BossHider();
        ~BossHider();
        bool init(Boss* boss);
        bool uninit();
        void update(f32 dt);
        void collide(f32 dt);

    private:
        void BossState_Idle_Update(f32 dt);
        void BossState_Escaping_Update(f32 dt);
        void BossState_Chasing_Update(f32 dt);

        enum
        {
            eBS_Invalid = 0,
            eBS_Idle = 1,
            eBS_Esacping = 2,
            eBS_Chasing = 3
        } m_curState;

        f32 m_godTime;
    };

}
