//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "Precompiled.h"
#include "Gameplay/BaseBossController.h"
#include "../MapDefinitions.h"

namespace Echo
{
    enum EBossRusherStatus
    {
        eBRS_Normal = 0,
        eBRS_Dash = 1,
        eBRS_Passout = 2,

        eBRS_Invalid
    };

    class BossRusher : public BaseBossController
    {
    public:
        BossRusher();
        ~BossRusher();
        bool init(Boss* boss);
        bool uninit();
        void update(f32 dt);
        void collide(f32 dt);

    private:
        void setPassoutTime(f32 time);
        void setLockOnTime(f32 time);
        void setDirection(EMoveDirection dir);
        EBossRusherStatus getBossStatus() const;

        bool brain_Normal();
        u32 brain_Dash();
        bool judgeBlocked();
        bool brainSelecetor(f32 dt);

    private:
        f32 m_passoutTime;
        f32 m_passoutRealTime;
        f32 m_lockOnTime;
        f32 m_lockOnRealTime;
        f32 m_bossGodTime;
        f32 m_heroGodTime;
        bool m_moving;
        f32 m_timer;
        EMoveDirection m_direction;
        EBossRusherStatus m_bossStatus;
    };

}