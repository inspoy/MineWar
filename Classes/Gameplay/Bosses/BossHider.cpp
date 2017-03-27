//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Bosses/BossHider.h"
#include "Precompiled.h"
#include "../Engine/AudioManager.h"
#include "Gameplay/Boss.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"

namespace Echo
{

    BossHider::BossHider()
        : m_curState(eBS_Invalid)
        , m_godTime(0.0f)
    {}

    BossHider::~BossHider()
    {}

    bool BossHider::init(Boss* boss)
    {
        auto ret = super::init(boss);

        // animation
        auto bossAniamtion = cocos2d::Animation::create();
        for (u32 i = 1;; ++i)
        {
            auto idStr = String(i, 4, '0');
            String fp = ECHO_STR("animation/hider/Hider%s.png", idStr.c_str());
            if (cocos2d::FileUtils::getInstance()->isFileExist(fp.c_str()))
            {
                bossAniamtion->addSpriteFrameWithFile(fp.c_str());
            }
            else
            {
                break;
            }
        }
        bossAniamtion->setDelayPerUnit(0.04f); // 25fps
        bossAniamtion->setLoops(1);
        auto bossAnimate = cocos2d::Animate::create(bossAniamtion);
        auto bossAnimateWithSound = cocos2d::Spawn::createWithTwoActions(
            bossAnimate,
            cocos2d::CallFunc::create([=](){
            AudioManager::getInstance()->playSE("hider_move");
        }));
        m_boss->getSprite()->runAction(cocos2d::RepeatForever::create(bossAnimateWithSound));

        // FSM: init state
        m_curState = eBS_Idle;

        return ret;
    }

    bool BossHider::uninit()
    {
        return super::uninit();
    }

    void BossHider::update(f32 dt)
    {
        super::update(dt);

        // FSM: update
        switch (m_curState)
        {
        case Echo::BossHider::eBS_Idle:
            BossState_Idle_Update(dt);
            break;
        case Echo::BossHider::eBS_Esacping:
            BossState_Escaping_Update(dt);
            break;
        case Echo::BossHider::eBS_Chasing:
            BossState_Chasing_Update(dt);
            break;
        default:
            ECHO_ASSERT(false, "Invalid boss state.");
            break;
        }
    }

    void BossHider::collide(f32 dt)
    {
        if (m_godTime == 0)
        {
            m_godTime = MetaDataHelper::getInstance()->getGodTime();

            m_gameMapController->reduceHp(m_boss->getDamage());
        }
        else
        {
            m_godTime -= dt;
            if (m_godTime < 0.0f)
            {
                m_godTime = 0.0f;
            }
        }
    }

    void BossHider::BossState_Idle_Update(f32 /*dt*/)
    {
        if (m_boss->getMoving()) return;

        auto path = pathFinding();
        // FSM: state transition
        if (path.size() > 2 && path.size() < 5)
        {
            m_curState = eBS_Esacping;
        }
        else if (path.size() > 0 && path.size() <= 2)
        {
            m_curState = eBS_Chasing;
        }
        else
        {
            m_boss->moveWithAnimation(eMD_Invalid);
        }
    }

    void BossHider::BossState_Escaping_Update(f32 /*dt*/)
    {
        if (m_boss->getMoving()) return;

        auto path = pathFinding();
        if (path.size() == 0 || path.size() >= 5)
        {
            m_curState = eBS_Idle;
        }
        else if (path.size() < 3)
        {
            m_curState = eBS_Chasing;
        }
        else
        {
            auto direction = path.front();
            direction = EMoveDirection((direction + 2) % 4);
            MapPoint np = m_boss->getPos().moveWithDirection(direction);
            auto ent = m_gameMapController->getOwner()->getEntity(np.m_x, np.m_y);
            if ((ent && ent->getBlock()) || !m_gameMapController->posValid(np))
            {
                direction = eMD_Invalid;
            }

            m_boss->moveWithAnimation(direction);
        }
    }

    void BossHider::BossState_Chasing_Update(f32 /*dt*/)
    {
        if (m_boss->getMoving()) return;

        auto path = pathFinding();
        if (path.size() == 0 || path.size() >= 5)
        {
            m_curState = eBS_Idle;
        }
        else if (path.size() > 2)
        {
            m_curState = eBS_Esacping;
        }
        else
        {
            m_boss->moveWithAnimation(path.front());
        }

    }

}