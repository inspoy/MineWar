//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once;

#include "Precompiled.h"

namespace Echo
{

    typedef std::function< void(f32 dt) > UpdateFunc;
    typedef std::function< void() > DefaultFunc;
    struct FsmState
    {
        FsmState()
        : m_c(false)
        , m_u(false)
        , m_d(false)
        {}

        bool m_c, m_u, m_d;
        DefaultFunc m_constructor;
        UpdateFunc m_update;
        DefaultFunc m_destructor;
    };

    class Fsm
    {
    public:
        Fsm();
        ~Fsm();

        void update(f32 dt);
        void addState(String stateName);
        void setDefaultState(String stateName);
        void setConstructor(String stateName, DefaultFunc func);
        void setUpdate(String stateName, UpdateFunc func);
        void setDestructor(String stateName, DefaultFunc func);
        void transitTo(String stateName);

    private:
        StdString m_curState;
        std::map<StdString, FsmState> m_states;
    };

}

// macros

#define ECHO_DECLARE_FSM(CLASS_NAME)\
    typedef CLASS_NAME Owner; \
    Fsm m_fsm

#define ECHO_DECLARE_STATE(STATE_NAME)\
    void FSM_State_##STATE_NAME##_Constructor(); \
    void FSM_State_##STATE_NAME##_Update(f32 dt); \
    void FSM_State_##STATE_NAME##_Destructor()

#define ECHO_CREATE_STATE(STATE_NAME)\
    m_fsm.addState(#STATE_NAME); \
    m_fsm.setConstructor(#STATE_NAME, CC_CALLBACK_1(Owner::FSM_State_##STATE_NAME##_Constructor(), this)); \
    m_fsm.setUpdate(#STATE_NAME, CC_CALLBACK_1(Owner::FSM_State_##STATE_NAME##_Update(), this)); \
    m_fsm.setDestructor(#STATE_NAME, CC_CALLBACK_1(Owner::FSM_State_##STATE_NAME##_Destructor(), this))

#define ECHO_INIT_STATE(STATE_NAME)\
    m_fsm.setDefaultState(#STATE_NAME)

#define ECHO_FSM_UPDATE(dt)\
    m_fsm.update(dt)

#define ECHO_IMPL_STATE_CONSTRUCTOR(CLASS_NAME, STATE_NAME)\
    void CLASS_NAME::FSM_State_##STATE_NAME##_Constructor()

#define ECHO_IMPL_STATE_UPDATE(CLASS_NAME, STATE_NAME)\
    void CLASS_NAME::FSM_State_##STATE_NAME##_Update(f32 dt)

#define ECHO_IMPL_STATE_DESTRUCTOR(CLASS_NAME, STATE_NAME)\
    void CLASS_NAME::FSM_State_##STATE_NAME##_Destructor()

#define ECHO_STATE_TRANSITION(cond, STATE_NAME)\
    if (cond)\
    {\
        m_fsm.transitTo(#STATE_NAME); \
    }
