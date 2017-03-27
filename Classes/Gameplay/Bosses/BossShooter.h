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

    class BossShooter : public BaseBossController
    {
    public:
        BossShooter();
        ~BossShooter();
        bool init(Boss* boss);
        bool uninit();
        void update(f32 dt);
        void collide(f32 dt);

    private:
        bool checkBlock();

        void BossState_Moving_Update(f32 dt);
        void BossState_Shooting_Update(f32 dt);

        enum
        {
            eBS_Invalid = 0,
            eBS_Moving = 1,
            eBS_Shooting = 2
        } m_curState;

        f32 m_curCD;
        static const f32 ARROW_CD;
        f32 m_godTime;
    };

}
