//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_UTILITIES_TIMEUTL_H__INCLUDED
#define ECHO_UTILITIES_TIMEUTL_H__INCLUDED

#include "Precompiled.h"

namespace Echo
{

    struct RunTime
    {
        RunTime()
        : m_hour(0)
        , m_min(0)
        , m_sec(0)
        {}

        RunTime(u32 h, u32 m, f32 s)
        : m_hour(h)
        , m_min(m)
        , m_sec(s)
        {}

        u32 m_hour;
        u32 m_min;
        f32 m_sec;

        RunTime operator-(const RunTime& arg);
        f32 toSec();
    };

    class TimeUtl
    {
    public:
        ECHO_DECLARE_SINGLETON(TimeUtl);

    public:
        RunTime getRunTime();
        String getRunTimeStr();
        void update(f32 dt);
        
    private:
        RunTime m_curRunTime;
    };

}

#endif