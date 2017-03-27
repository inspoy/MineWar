//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Engine/FSM.h"
#include "Precompiled.h"

namespace Echo
{

    Fsm::Fsm()
    {}

    Fsm::~Fsm()
    {}

    void Fsm::update(f32 dt)
    {
        ECHO_ASSERT(m_curState != "", "Must set default state!");
        m_states[m_curState].m_update(dt);
    }

    void Fsm::addState(String stateName)
    {
        ECHO_ASSERT(m_states.find(stateName.stdStr) == m_states.end(), "State alreay exist!");
        FsmState newState;
        m_states[stateName.stdStr] = newState;
    }

    void Fsm::setDefaultState(String stateName)
    {
        ECHO_ASSERT(m_states.find(stateName.stdStr) != m_states.end(), "Unknown State!");
        m_curState = stateName.stdStr;
    }

    void Fsm::setConstructor(String stateName, DefaultFunc func)
    {
        ECHO_ASSERT(m_states.find(stateName.stdStr) != m_states.end(), "Unknown State!");
        m_states[stateName.stdStr].m_constructor = func;
        m_states[stateName.stdStr].m_c = true;
    }

    void Fsm::setUpdate(String stateName, UpdateFunc func)
    {
        ECHO_ASSERT(m_states.find(stateName.stdStr) != m_states.end(), "Unknown State!");
        m_states[stateName.stdStr].m_update = func;
        m_states[stateName.stdStr].m_u = true;
    }

    void Fsm::setDestructor(String stateName, DefaultFunc func)
    {
        ECHO_ASSERT(m_states.find(stateName.stdStr) != m_states.end(), "Unknown State!");
        m_states[stateName.stdStr].m_destructor = func;
        m_states[stateName.stdStr].m_d = true;
    }

    void Fsm::transitTo(String stateName)
    {
        ECHO_ASSERT(m_states.find(stateName.stdStr) != m_states.end(), "Unknown State!");
        if (m_states[m_curState].m_d)
        {
            m_states[m_curState].m_destructor();
        }
        m_curState = stateName.stdStr;
        if (m_states[m_curState].m_c)
        {
            m_states[m_curState].m_constructor();
        }
    }

}