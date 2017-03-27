//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_ARROW_H__INCLUDED
#define ECHO_GAMEPLAY_ARROW_H__INCLUDED

#include "Gameplay/Item.h"
#include "Precompiled.h"

namespace Echo
{
    class Arrow : public Entity
    {
    public:
        Arrow();
        ~Arrow();

        static Arrow* create(String id);

        bool getMoving() const;
        Echo::f32 getMoveDuration() const;
        void setMoveDuration(Echo::f32 val);
        void setDamage(f32 amount);
        f32 getDamage() const;

        bool moveWithAnimation(EMoveDirection direction);

    private:
        void onMoveEnded();

    private:
        // if the arrow is moving
        bool m_moving;
        // 
        f32 m_moveDuration;
        f32 m_damage;
    };
}

#endif