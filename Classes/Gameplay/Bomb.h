//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_BOMB_H__INCLUDED
#define ECHO_GAMEPLAY_BOMB_H__INCLUDED

#include "Precompiled.h"
#include "Item.h"

namespace Echo
{

    enum EBombStatus
    {
        eBS_Idle = 0,
        eBS_InQte = 1,
        eBS_InCooldown = 2,
        eBS_InSelect = 3,
        eBS_InPlayerAct = 4,
        eBS_InRest = 5
    };

    class Bomb : public Entity
    {
    public:
        Bomb();
        ~Bomb();

        void init();
        void update(f32 dt);
        void startQte();

        void setCooldown(f32 val);

        void explode();


    protected:
        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);

        bool m_touching;

        EBombStatus m_status;
        f32 m_curCooldown;
        f32 m_cooldown;
        f32 m_curLoopTime;
        f32 m_loopTime;
        f32 m_randomTime;
        f32 m_curRestTime;
        f32 m_restTime;
        u32 m_dmgCount;
        u32 m_curRound;
        u32 m_round;
        u32 m_playerOperated;// 0 = no operation;  1 = right operation; 2 = wrong operation;
        EMoveDirection m_curDirection;

    private:
        cocos2d::EventListenerTouchOneByOne* m_touchListener;

        static const u32 BOMB_DAMAGE;
    };

}

#endif