//----------------------------------------------------------
//
// Echo Works @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_UTILITIES_TRACKEVENT_H__INCLUDED
#define ECHO_UTILITIES_TRACKEVENT_H__INCLUDED

#include "Precompiled.h"
#include "TimeUtl.h"

namespace Echo
{

    struct GameEvent
    {
        GameEvent()
        : m_id(0)
        {}

        u32     m_id;
        String  m_name;
        String  m_type;
        RunTime m_beginTime;
        RunTime m_endTime;
        RunTime m_lastTime;
        std::map<StdString, StdString>  m_arguments;

        bool start();
        bool finish();
    };

    class TrackEventManager
    {
    public:
        ECHO_DECLARE_SINGLETON(TrackEventManager);

    public:
        u32 createEvent(String eventName, String eventType);
        void destroyEvent(u32 eventId);
        bool startEvent(u32 eventId);
        bool addEventArugument(u32 eventId, String key, String value);
        bool finishEvent(u32 eventId);
        GameEvent* findEvent(u32 eventId);

    private:
        std::map<u32, GameEvent*> m_gameEvents;
        u32 m_eventCount;
    };

}

#endif