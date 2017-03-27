//----------------------------------------------------------
//
// Echo Works @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Utilities/TrackEvent.h"

namespace Echo
{

    ECHO_IMPLEMENT_SINGLETON(TrackEventManager);

    TrackEventManager::TrackEventManager()
        : m_eventCount(0)
    {}

    TrackEventManager::~TrackEventManager()
    {}

    Echo::u32 TrackEventManager::createEvent(String eventName, String eventType)
    {
        GameEvent* newEvent = new GameEvent();
        u32 newId = ++m_eventCount;
        newEvent->m_id = newId;
        m_gameEvents[newId] = newEvent;
        return newId;
    }

    void TrackEventManager::destroyEvent(u32 eventId)
    {
        auto target = findEvent(eventId);

        if (target)
        {
            CC_SAFE_DELETE(target);
            m_gameEvents[eventId] = NULL;
        }
    }

    bool TrackEventManager::startEvent(u32 eventId)
    {
        auto target = findEvent(eventId);

        if (!target) return false;

        return target->start();
    }

    bool TrackEventManager::addEventArugument(u32 eventId, String key, String value)
    {
        auto target = findEvent(eventId);

        if (!target) return false;
        if (target->m_arguments.find(key.stdStr) == target->m_arguments.end()) return false;

        target->m_arguments[key.stdStr] = value.stdStr;

        return true;
    }

    bool TrackEventManager::finishEvent(u32 eventId)
    {
        auto target = findEvent(eventId);

        if (!target) return false;

        return target->finish();
    }

    Echo::GameEvent* TrackEventManager::findEvent(u32 eventId)
    {
        GameEvent* ret = NULL;

        if (m_gameEvents.find(eventId) != m_gameEvents.end())
        {
            ret = m_gameEvents[eventId];
        }

        return ret;
    }

    bool GameEvent::start()
    {
        m_beginTime = Echo::TimeUtl::getInstance()->getRunTime();

        return true;
    }

    bool GameEvent::finish()
    {
        m_endTime = Echo::TimeUtl::getInstance()->getRunTime();
        m_lastTime = m_endTime - m_beginTime;

        return true;
    }

}