//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_BOSS_H__INCLUDED
#define ECHO_GAMEPLAY_BOSS_H__INCLUDED

#include "Gameplay/Item.h"
#include "Precompiled.h"
#include "BaseBossController.h"

namespace Echo
{
    class Boss : public Entity
    {
    public:
        Boss();
        ~Boss();

        bool moveWithAnimation(EMoveDirection direction);
        bool getMoving() const;
        Echo::f32 getMoveDuration() const;
        void setMoveDuration(Echo::f32 val);
        void resetMoveDuration();
        void setHP(s32 HP);
        s32 getHP() const;
        void setDamage(f32 val);
        f32 getDamage() const;
        void setOwner(BaseBossController* owner);
        BaseBossController* getOwner() const;

    private:
        void onMoveEnded();

    private:
        bool m_moving;
        s32 m_healthPoint;
        f32 m_moveDuration;
        f32 m_originalDuration;
        f32 m_damage;
        BaseBossController* m_owner;
    };
}

#endif