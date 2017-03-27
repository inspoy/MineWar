//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Bosses/BossRusher.h"
#include "Precompiled.h"
#include "Scenes/GameMapController.h"
#include "../Boss.h"
#include "Scenes/SceneGameMap.h"
#include "../Engine/AudioManager.h"

namespace Echo
{

    BossRusher::BossRusher()
        : m_passoutTime(0.0f)
        , m_passoutRealTime(0.0f)
        , m_lockOnTime(0.0f)
        , m_lockOnRealTime(0.0f)
        , m_bossGodTime(0.0f)
        , m_heroGodTime(0.0f)
        , m_moving(false)
        , m_timer(0.0f)
        , m_direction(eMD_Invalid)
        , m_bossStatus(eBRS_Invalid)
    {}

    BossRusher::~BossRusher()
    {}

    bool BossRusher::init(Boss* boss)
    {
        bool ret = super::init(boss);
        m_bossStatus = eBRS_Normal;
        setLockOnTime(2.0f);
        setPassoutTime(2.0f);

        // animation
        auto bossAniamtion = cocos2d::Animation::create();
        for (u32 i = 1;; ++i)
        {
            auto idStr = String(i, 4, '0');
            String fp = ECHO_STR("animation/rusher/Rusher%s.png", idStr.c_str());
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
            AudioManager::getInstance()->playSE("rusher_move");
        }));
        m_boss->getSprite()->runAction(cocos2d::RepeatForever::create(bossAnimateWithSound));

        return ret;
    }

    bool BossRusher::uninit()
    {
        return super::uninit();
    }

    void BossRusher::update(f32 dt)
    {
        super::update(dt);
        m_moving = m_boss->getMoving();
        m_direction = EMoveDirection(m_boss->getDirection());
        if (!brainSelecetor(dt))
        {
            ECHO_TRACE_NOARG("Boss", "You dead...");
        }
    }

    void BossRusher::collide(f32 dt)
    {
        if (m_bossGodTime == 0 && m_heroGodTime == 0)
        {
            m_bossGodTime = MetaDataHelper::getInstance()->getGodTime();
            m_heroGodTime = MetaDataHelper::getInstance()->getGodTime();
            if (getBossStatus() == eBRS_Passout)
            {
                s32 HP = m_boss->getHP() - MetaDataHelper::getInstance()->getHeroDamage();
                ECHO_TRACE("Boss", "The boss have %d HP left !", HP);
                if (HP <= 0)
                {
                    m_boss->getOwner()->killBoss();
                }
                else
                {
                    m_boss->setHP(HP);
                }
            }
            else
            {
                m_gameMapController->reduceHp(m_boss->getDamage());
            }
        }
        else
        {
            m_bossGodTime -= dt;
            if (m_bossGodTime < 0.0f)
            {
                m_bossGodTime = 0.0f;
            }
            m_heroGodTime -= dt;
            if (m_heroGodTime)
            {
                m_heroGodTime = 0.0f;
            }
        }
    }

    void BossRusher::setPassoutTime(f32 time)
    {
        ECHO_ASSERT(time > 0, "The BOSS's passout time is wrong !");
        m_passoutTime = time;
    }

    void BossRusher::setLockOnTime(f32 time)
    {
        ECHO_ASSERT(time >= 0, "The BOSS's lock on time is wrong !");
        m_lockOnTime = time;
    }

    Echo::EBossRusherStatus BossRusher::getBossStatus() const
    {
        return m_bossStatus;
    }

    bool BossRusher::brain_Normal()
    {
        ECHO_ASSERT(m_gameMapController != NULL, "Can't get GameMapController");

        s32 dx = m_gameMapController->getHeroCurPosX() - m_bossPosX;
        s32 dy = m_gameMapController->getHeroCurPosY() - m_bossPosY;


        if (dx >= 0)
        {
            if (dy >= 0)
            {
                if (dx == 0 && dy == 0)
                {
                    return false;
                }

                //1
                bool tFlag = true;
                bool rFlag = true;
                for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                {
                    if (curEnt->getBlock() == true)
                    {
                        if (curEnt->getPosX() == m_bossPosX + 1 && curEnt->getPosY() == m_bossPosY)
                        {
                            rFlag = false;
                        }
                        else if (curEnt->getPosX() == m_bossPosX && curEnt->getPosY() == m_bossPosY + 1)
                        {
                            tFlag = false;
                        }
                    }
                }

                if (m_bossPosY == m_gameMapController->getMapHeight() - 1)
                {
                    tFlag = false;
                }
                else if (m_bossPosX == m_gameMapController->getMapWidth() - 1)
                {
                    rFlag = false;
                }

                if (dx == 0)
                {
                    rFlag = false;
                }
                else if (dy == 0)
                {
                    tFlag = false;
                }

                if (tFlag && rFlag)
                {
                    if (dx - dy >= 0)
                    {
                        rFlag = false;
                    }
                    else
                    {
                        tFlag = false;
                    }
                }

                if (tFlag)
                {
                    // go top
                    m_boss->moveWithAnimation(eMD_Top);
                }
                else if (rFlag)
                {
                    // go right
                    m_boss->moveWithAnimation(eMD_Right);
                }
            }
            else
            {
                //4
                bool bFlag = true;
                bool rFlag = true;
                for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                {
                    if (curEnt->getBlock() == true)
                    {
                        if (curEnt->getPosX() == m_bossPosX + 1 && curEnt->getPosY() == m_bossPosY)
                        {
                            rFlag = false;
                        }
                        else if (curEnt->getPosX() == m_bossPosX && curEnt->getPosY() == m_bossPosY - 1)
                        {
                            bFlag = false;
                        }
                    }
                }

                if (m_bossPosY == 1)
                {
                    bFlag = false;
                }
                else if (m_bossPosX == m_gameMapController->getMapWidth() - 1)
                {
                    rFlag = false;
                }

                if (dx == 0)
                {
                    rFlag = false;
                }
                else if (dy == 0)
                {
                    bFlag = false;
                }

                if (bFlag && rFlag)
                {
                    if (dx + dy >= 0)
                    {
                        rFlag = false;
                    }
                    else
                    {
                        bFlag = false;
                    }
                }

                if (bFlag)
                {
                    // go bottom
                    m_boss->moveWithAnimation(eMD_Bottom);
                }
                else if (rFlag)
                {
                    // go right
                    m_boss->moveWithAnimation(eMD_Right);
                }
            }
        }
        else
        {
            if (dy >= 0)
            {
                //2
                bool tFlag = true;
                bool lFlag = true;
                for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                {
                    if (curEnt->getBlock() == true)
                    {
                        if (curEnt->getPosX() == m_bossPosX - 1 && curEnt->getPosY() == m_bossPosY)
                        {
                            lFlag = false;
                        }
                        else if (curEnt->getPosX() == m_bossPosX && curEnt->getPosY() == m_bossPosY + 1)
                        {
                            tFlag = false;
                        }
                    }
                }

                if (m_bossPosY == m_gameMapController->getMapHeight() - 1)
                {
                    tFlag = false;
                }
                else if (m_bossPosX == 1)
                {
                    lFlag = false;
                }

                if (dx == 0)
                {
                    lFlag = false;
                }
                else if (dy == 0)
                {
                    tFlag = false;
                }

                if (tFlag && lFlag)
                {
                    if (dx + dy >= 0)
                    {
                        tFlag = false;
                    }
                    else
                    {
                        lFlag = false;
                    }
                }

                if (tFlag)
                {
                    // go top
                    m_boss->moveWithAnimation(eMD_Top);
                }
                else if (lFlag)
                {
                    // go left
                    m_boss->moveWithAnimation(eMD_Left);
                }
            }
            else
            {
                //3
                bool bFlag = true;
                bool lFlag = true;
                for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                {
                    if (curEnt->getBlock() == true)
                    {
                        if (curEnt->getPosX() == m_bossPosX - 1 && curEnt->getPosY() == m_bossPosY)
                        {
                            lFlag = false;
                        }
                        else if (curEnt->getPosX() == m_bossPosX && curEnt->getPosY() == m_bossPosY - 1)
                        {
                            bFlag = false;
                        }
                    }
                }

                if (m_bossPosY == 1)
                {
                    bFlag = false;
                }
                else if (m_bossPosX == 1)
                {
                    lFlag = false;
                }

                if (dx == 0)
                {
                    lFlag = false;
                }
                else if (dy == 0)
                {
                    bFlag = false;
                }

                if (bFlag && lFlag)
                {
                    if (dx - dy >= 0)
                    {
                        bFlag = false;
                    }
                    else
                    {
                        lFlag = false;
                    }
                }

                if (bFlag)
                {
                    // go bottom
                    m_boss->moveWithAnimation(eMD_Bottom);
                }
                else if (lFlag)
                {
                    // go left
                    m_boss->moveWithAnimation(eMD_Left);
                }
            }
        }

        return true;
    }

    Echo::u32 BossRusher::brain_Dash()
    {
        if (m_gameMapController->getHeroCurPosX() == m_bossPosX && m_gameMapController->getHeroCurPosY() == m_bossPosY)
        {
            return 1;// hit hero
        }
        else
        {
            switch (m_direction)
            {
            case Echo::eMD_Top:
                if (m_bossPosY == m_gameMapController->getMapHeight() - 1)
                {
                    return 0;
                }
                else
                {
                    for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                    {
                        if (curEnt->getBlock() == true)
                        {
                            if (curEnt->getPosX() == m_bossPosX && curEnt->getPosY() == m_bossPosY + 1)
                            {
                                return 0;
                            }
                        }
                    }

                    m_boss->moveWithAnimation(eMD_Top);

                    return 2;
                }
                break;
            case Echo::eMD_Left:
                if (m_bossPosX == 1)
                {
                    return 0;
                }
                else
                {
                    for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                    {
                        if (curEnt->getBlock() == true)
                        {
                            if (curEnt->getPosX() == m_bossPosX - 1 && curEnt->getPosY() == m_bossPosY)
                            {
                                return 0;
                            }
                        }
                    }

                    m_boss->moveWithAnimation(eMD_Left);

                    return 2;
                }
                break;
            case Echo::eMD_Bottom:
                if (m_bossPosY == 1)
                {
                    return 0;
                }
                else
                {
                    for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                    {
                        if (curEnt->getBlock() == true)
                        {
                            if (curEnt->getPosX() == m_bossPosX && curEnt->getPosY() == m_bossPosY - 1)
                            {
                                return 0;
                            }
                        }
                    }

                    m_boss->moveWithAnimation(eMD_Bottom);

                    return 2;
                }
                break;
            case Echo::eMD_Right:
                if (m_bossPosX == m_gameMapController->getMapWidth() - 1)
                {
                    return 0;
                }
                else
                {
                    for (auto curEnt : (m_gameMapController->getOwner()->getCurEntities()))
                    {
                        if (curEnt->getBlock() == true)
                        {
                            if (curEnt->getPosX() == m_bossPosX + 1 && curEnt->getPosY() == m_bossPosY)
                            {
                                return 0;
                            }
                        }
                    }

                    m_boss->moveWithAnimation(eMD_Right);

                    return 2;
                }
                break;
            default:
                ECHO_TRACE_NOARG("Boss", "BOSS's direction is wrong !");
                return 2;
                break;
            }
        }
    }

    bool BossRusher::judgeBlocked()
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

    bool BossRusher::brainSelecetor(f32 dt)
    {
        if (m_moving)
        {
            m_timer += dt;
            return true;
        }
        else if (m_timer > MATH_EPSILON)
        {
            dt = m_timer;
            m_timer = 0.0f;
        }


        bool retVal = false;

        u32 midStatus = 3;

        switch (m_bossStatus)
        {
        case Echo::eBRS_Normal:
            if (!judgeBlocked())
            {
                m_lockOnRealTime += dt;
                if (m_lockOnRealTime >= m_lockOnTime)
                {
                    m_lockOnRealTime = 0.0f;
                    m_bossStatus = eBRS_Dash;
                    m_boss->setMoveDuration(m_boss->getMoveDuration() / 4.0f);
                    midStatus = brain_Dash();
                    if (midStatus == 0)// hit something
                    {
                        m_bossStatus = eBRS_Passout;
                        retVal = true;
                    }
                    else if (midStatus == 1)// hit hero
                    {
                        m_bossStatus = eBRS_Normal;
                        m_boss->resetMoveDuration();
                        retVal = false;
                    }
                    else if (midStatus == 2)// reach nothing
                    {
                        retVal = true;
                    }
                    else
                    {
                        ECHO_ASSERT(false, "The value of barin_Dash() is wrong !");
                    }
                }
                else
                {
                    retVal = brain_Normal();
                }
            }
            else
            {
                m_lockOnRealTime = 0.0f;
                retVal = brain_Normal();
            }
            break;
        case Echo::eBRS_Dash:
            midStatus = brain_Dash();
            if (midStatus == 0)// hit something
            {
                m_bossStatus = eBRS_Passout;
                retVal = true;
            }
            else if (midStatus == 1)// hit hero
            {
                m_bossStatus = eBRS_Normal;
                m_boss->resetMoveDuration();
                retVal = false;
            }
            else if (midStatus == 2)// reach nothing
            {
                retVal = true;
            }
            else
            {
                ECHO_TRACE_NOARG("Boss", "The value of barin_Dash() is wrong !");
            }
            break;
        case Echo::eBRS_Passout:
            m_passoutRealTime += dt;
            if (m_passoutRealTime >= m_passoutTime)
            {
                m_passoutRealTime = 0.0f;
                m_bossStatus = eBRS_Normal;
                m_boss->resetMoveDuration();
                retVal = brain_Normal();
            }
            else
            {
                retVal = true;
            }

            break;
        default:
            ECHO_TRACE_NOARG("Boss", "BOSS's status is wrong !");
            retVal = true;
            break;
        }

        return retVal;
    }

    void BossRusher::setDirection(EMoveDirection dir)
    {
        m_direction = dir;
    }

}