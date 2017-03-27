//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Boss.h"
#include "Precompiled.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Scenes/GameMapController.h"

namespace Echo
{

    Boss::Boss()
        : m_moving(false)
        , m_moveDuration(0.0f)
        , m_originalDuration(0.0f)
    {
        m_found = true;
        m_hide = false;
    }

    Boss::~Boss()
    {

    }

    bool Boss::getMoving() const
    {
        return m_moving;
    }

    void Boss::onMoveEnded()
    {
        m_moving = false;
    }

    bool Boss::moveWithAnimation(EMoveDirection direction)
    {
        if (m_moving) return false;
        ECHO_ASSERT(m_moveDuration > MATH_EPSILON, "the m_moveDuration has not been initialized.");

        switch (direction)
        {
        case Echo::eMD_Top:
            m_pos.m_y += 1;
            setDirection(0);
            break;
        case Echo::eMD_Left:
            m_pos.m_x -= 1;
            setDirection(1);
            break;
        case Echo::eMD_Bottom:
            m_pos.m_y -= 1;
            setDirection(2);
            break;
        case Echo::eMD_Right:
            m_pos.m_x += 1;
            setDirection(3);
            break;
        case Echo::eMD_Invalid:
            break;
        default:
            ECHO_ASSERT(false, "");
            break;
        }

        cocos2d::Vec2 goalPos = GameMapController::getRealPos(m_pos);
        auto moveAction = cocos2d::MoveTo::create(m_moveDuration, goalPos);
        m_sprite->runAction(cocos2d::Sequence::createWithTwoActions(moveAction, cocos2d::CallFunc::create(CC_CALLBACK_0(Boss::onMoveEnded, this))));
        m_moving = true;
        return true;
    }

    Echo::f32 Boss::getMoveDuration() const
    {
        return m_moveDuration;
    }

    void Boss::setMoveDuration(Echo::f32 val)
    {
        if (m_moveDuration < MATH_EPSILON)
        {
            m_originalDuration = val;
        }

        m_moveDuration = val;
    }

    void Boss::resetMoveDuration()
    {
        m_moveDuration = m_originalDuration;
    }

    void Boss::setHP(s32 HP)
    {
        m_healthPoint = HP;
    }

    Echo::s32 Boss::getHP() const
    {
        return m_healthPoint;
    }

    void Boss::setOwner(BaseBossController* owner)
    {
        m_owner = owner;
    }

    BaseBossController* Boss::getOwner() const
    {
        return m_owner;
    }

    void Boss::setDamage(f32 val)
    {
        m_damage = val;
    }

    Echo::f32 Boss::getDamage() const
    {
        return m_damage;
    }

}