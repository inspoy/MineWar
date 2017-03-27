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

    class BossBoomer : public BaseBossController
    {
    public:
        BossBoomer();
        ~BossBoomer();
        bool init(Boss* boss);
        bool uninit();
        void update(f32 dt);
        void collide(f32 dt);

    private:
        void BossState_Running_Update(f32 dt);
        void BossState_Exploding_Update(f32 dt);
        void BossState_Dead_Update(f32 dt);

        enum
        {
            eBS_Invalid = 0,
            eBS_Running = 1,
            eBS_Exploding = 2,
            eBS_Dead = 3
        } m_curState;
        f32 m_explodeTimer;

        static const f32 THRESHHOLD_EXPLODE;
    };

}