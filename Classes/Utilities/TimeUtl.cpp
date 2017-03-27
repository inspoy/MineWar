//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Utilities/TimeUtl.h"

namespace Echo
{

    ECHO_IMPLEMENT_SINGLETON(TimeUtl);

    TimeUtl::TimeUtl()
    {}

    TimeUtl::~TimeUtl()
    {}

    Echo::RunTime TimeUtl::getRunTime()
    {
        return m_curRunTime;
    }

    String TimeUtl::getRunTimeStr()
    {
        return ECHO_STR("%d:%d:%.2f", m_curRunTime.m_hour, m_curRunTime.m_min, m_curRunTime.m_sec);
    }

    void TimeUtl::update(f32 dt)
    {
        m_curRunTime.m_sec += dt;
        while (m_curRunTime.m_sec > 60.0f)
        {
            m_curRunTime.m_sec -= 60.0f;
            m_curRunTime.m_min += 1;
            while (m_curRunTime.m_min > 60)
            {
                m_curRunTime.m_min -= 60;
                m_curRunTime.m_hour += 1;
            }
        }
    }

    Echo::RunTime RunTime::operator-(const RunTime& arg)
    {
        u32 newH = m_hour;
        u32 newM = m_min;
        f32 newS = m_sec;

        if (newS < arg.m_sec)
        {
            newS += 60.0f;
            newM -= 1;
        }
        newS -= arg.m_sec;

        if (newM < arg.m_min)
        {
            newM += 60.0f;
            newH -= 1;
        }
        newM -= arg.m_min;

        newH -= arg.m_hour;

        return RunTime(newH, newM, newS);
    }

    Echo::f32 RunTime::toSec()
    {
        return m_sec + m_min * 60 + m_hour * 3600;
    }

}