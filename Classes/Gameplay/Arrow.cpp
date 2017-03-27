//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Arrow.h"
#include "Precompiled.h"
#include "Scenes\GameMapController.h"
#include "Engine\GameLogic.h"

namespace Echo
{
    Arrow::Arrow()
        : m_moving(false)
        , m_moveDuration(0.0f)
        , m_damage(0.0f)
    {
    }

    Arrow::~Arrow()
    {
    }

    bool Arrow::getMoving() const
    {
        return m_moving;
    }

    bool Arrow::moveWithAnimation(EMoveDirection direction)
    {
        if (m_moving) return false;
        ECHO_ASSERT(m_moveDuration > MATH_EPSILON, "the m_moveDuration has not been initialized.");

        switch (direction)
        {
        case Echo::eMD_Top:
            m_pos.m_y += 1;
            break;
        case Echo::eMD_Left:
            m_pos.m_x -= 1;
            break;
        case Echo::eMD_Bottom:
            m_pos.m_y -= 1;
            break;
        case Echo::eMD_Right:
            m_pos.m_x += 1;
            break;
        default:
            ECHO_ASSERT(false, "");
            break;
        }

        cocos2d::Vec2 goalPos = GameMapController::getRealPos(m_pos);

        auto moveAction = cocos2d::MoveTo::create(m_moveDuration, goalPos);
        m_sprite->runAction(cocos2d::Sequence::createWithTwoActions(moveAction, cocos2d::CallFunc::create(CC_CALLBACK_0(Arrow::onMoveEnded, this))));
        m_moving = true;
        return true;
    }

    void Arrow::onMoveEnded()
    {
        m_moving = false;
    }

    Echo::f32 Arrow::getMoveDuration() const
    {
        return m_moveDuration;
    }

    void Arrow::setMoveDuration(Echo::f32 val)
    {
        m_moveDuration = val;
    }

    Arrow* Arrow::create(String id)
    {
        Arrow* ret = new Arrow();
        ret->m_sprite = cocos2d::Sprite::create(ECHO_STR("entities/%s.png", id.c_str()));
        ECHO_ASSERT(ret->m_sprite != NULL, "Invalid item id: %s", id.c_str());
        ret->m_uid = GameLogic::getInstance()->getNewUid();
        return ret;
    }

    void Arrow::setDamage(f32 amount)
    {
        ECHO_ASSERT(amount > 0, "The amount of damage setting is not right!");
        m_damage = amount;
    }

    Echo::f32 Arrow::getDamage() const
    {
        ECHO_ASSERT(m_damage > 0, "The amount of damage is not right!");
        return m_damage;
    }

}