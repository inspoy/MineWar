//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_KEYANDDOOR_H__INCLUDED
#define ECHO_GAMEPLAY_KEYANDDOOR_H__INCLUDED

#include "Gameplay/Item.h"
#include "Precompiled.h"

namespace Echo
{
    class KeyAndDoor : public Entity
    {
    public:
        KeyAndDoor();
        ~KeyAndDoor();

        void update(f32 dt);

        f32 getCurOpenTime() const;
        void setCurOpenTime(f32 val);
        void resetCurOpenTime();
        void setOpenTime(f32 val);

        u32 getKeyTag() const;
        void setKeyTag(u32 kTag);
        void setFileName(String fileName);

        void open(KeyAndDoor* theKey);

    private:
        u32 m_kTag;
        f32 m_curOpenTime;
        f32 m_totalOpenTime;
        String m_fileName;
    };
}

#endif