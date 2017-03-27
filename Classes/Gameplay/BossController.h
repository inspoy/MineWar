//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_BOSSCONTROLLER_H__INCLUDED
#define ECHO_GAMEPLAY_BOSSCONTROLLER_H__INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "MapDefinitions.h"



namespace Echo
{

    enum EBossRusherStatus
    {
        eBRS_Normal = 0,
        eBRS_Dash = 1,
        eBRS_Passout = 2,

        eBRS_Invalid
    };
    
    class GameMapController;
    class Boss;

    class BossController
    {
    public:
        BossController();
        ~BossController();

        bool init(Boss* boss);
        void update(f32 dt);

        void setGameMapController(GameMapController* controller);
        void setBossDirection(EMoveDirection dir);
        void setPassoutTime(f32 pTime);
        void setLockOnTime(f32 lTime);
        EBossRusherStatus getBossStatus() const;

        bool brain_Normal();
        u32 brain_Dash();
        bool judgeBlocked();

        bool brainSelector(f32 dt);

    private:
        GameMapController* m_gameMapController;
        Boss* m_boss;

        u32 m_bossPosX;
        u32 m_bossPosY;
        f32 m_passoutTime;
        f32 m_passoutRealTime;
        f32 m_lockOnTime;
        f32 m_lockOnRealTime;
        bool m_moving;
        EMoveDirection m_bossDirection;
        f32 m_timer;

        EBossRusherStatus m_bossStatus;
        
    };

}

#endif
