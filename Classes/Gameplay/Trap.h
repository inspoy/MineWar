//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_TRAP_H__INCLUDED
#define ECHO_GAMEPLAY_TRAP_H__INCLUDED

#include "Gameplay/Item.h"
#include "Precompiled.h"

namespace Echo
{
    class Trap : public Entity
    {
    public:
        Trap();
        ~Trap();

        bool updateCooldown(f32 dt);
        void setCooldown(f32 time);

    private:
        f32 m_cooldown;
        f32 m_passedTime;
    };
}

#endif