//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "Precompiled.h"
#include "Item.h"

namespace Echo
{

    class MyBomb : public Entity
    {
    public:
        MyBomb();
        ~MyBomb();

        void init();
        void update(f32 dt);

    private:
        f32 m_boomTime;
        f32 m_curBoomTime;

        u32 m_dmgPoint;

        void explode();
    };

}