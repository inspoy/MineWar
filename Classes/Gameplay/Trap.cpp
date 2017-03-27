//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Trap.h"
#include "Precompiled.h"

namespace Echo
{
    Trap::Trap()
        : m_passedTime(0)
        , m_cooldown(0.0f)
    {
    }

    Trap::~Trap()
    {
    }

    bool Trap::updateCooldown(f32 dt)
    {
        m_passedTime += dt;
        if (m_passedTime >= m_cooldown)
        {
            m_passedTime -= m_cooldown;
            return true;
        }
        return false;
    }

    void Trap::setCooldown(f32 time)
    {
        m_cooldown = time;
    }

}