//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Bosses/BossShooter.h"
#include "Precompiled.h"
#include "../Engine/AudioManager.h"
#include "Gameplay/Boss.h"
#include "../Engine/MetaDataHelper.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "../ArrowManager.h"

namespace Echo
{

    const f32 BossShooter::ARROW_CD = 1.0f;

    BossShooter::BossShooter()
        : m_curState(eBS_Invalid)
        , m_curCD(0.0f)
        , m_godTime(0.0f)
    {}

    BossShooter::~BossShooter()
    {}

    bool BossShooter::init(Boss* boss)
    {
        auto ret = super::init(boss);

        // animation
        auto bossAniamtion = cocos2d::Animation::create();
        for (u32 i = 1;; ++i)
        {
            auto idStr = String(i, 4, '0');
            String fp = ECHO_STR("animation/shooter/Shooter%s.png", idStr.c_str());
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
            AudioManager::getInstance()->playSE("shooter_move");
        }));
        m_boss->getSprite()->runAction(cocos2d::RepeatForever::create(bossAnimateWithSound));

        // FSM: init state
        m_curState = eBS_Moving;

        return ret;
    }

    bool BossShooter::uninit()
    {
        return super::uninit();
    }

    void BossShooter::update(f32 dt)
    {
        super::update(dt);

        if (m_curCD > 0.0f)
        {
            m_curCD -= dt;
        }

        // FSM: update
        switch (m_curState)
        {
        case Echo::BossShooter::eBS_Moving:
            BossState_Moving_Update(dt);
            break;
        case Echo::BossShooter::eBS_Shooting:
            BossState_Shooting_Update(dt);
            break;
        default:
            ECHO_ASSERT(false, "Invalid boss state.");
            break;
        }
    }

    void BossShooter::collide(f32 dt)
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

    void BossShooter::BossState_Moving_Update(f32 /*dt*/)
    {
        if (m_boss->getMoving()) return;

        auto path = pathFinding();
        if (path.size() > 5)
        {
            m_boss->moveWithAnimation(path.front());
        }
        else if (path.size() == 5 || path.size() == 0)
        {
            m_boss->moveWithAnimation(eMD_Invalid);
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

        if (m_curCD < MATH_EPSILON)
        {
            if (!checkBlock())
            {
                m_curState = eBS_Shooting;
                m_boss->setDirection(path.front());
            }
        }

    }

    void BossShooter::BossState_Shooting_Update(f32 /*dt*/)
    {
        if (m_boss->getMoving()) return;
        if (m_curCD > MATH_EPSILON)
        {
            m_curState = eBS_Moving;
            return;
        }

        if (checkBlock())
        {
            m_curState = eBS_Moving;
        }
        else
        {
            m_gameMapController->getArrowManager()->addArrow(m_boss);
            m_curCD = ARROW_CD;
        }
    }

    bool BossShooter::checkBlock()
    {

        if (m_gameMapController->getHeroCurPosX() == m_bossPosX)
        {
            for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
            {
                if (curEnt->getBlock() == true)
                {
                    if ((curEnt->getPosY() > m_bossPosY && curEnt->getPosY() < m_gameMapController->getHeroCurPosY()
                        || curEnt->getPosY() < m_bossPosY && curEnt->getPosY() > m_gameMapController->getHeroCurPosY())
                        && curEnt->getPosX() == m_bossPosX)
                    {
                        return true;
                    }
                }
            }

            return false;
        }
        else if (m_gameMapController->getHeroCurPosY() == m_bossPosY)
        {
            for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
            {
                if (curEnt->getBlock() == true)
                {
                    if ((curEnt->getPosX() > m_bossPosX && curEnt->getPosX() < m_gameMapController->getHeroCurPosX()
                        || curEnt->getPosX() < m_bossPosX && curEnt->getPosX() > m_gameMapController->getHeroCurPosX())
                        && curEnt->getPosY() == m_bossPosY)
                    {
                        return true;
                    }
                }
            }

            return false;
        }
        else
        {
            return true;
        }
    }

}