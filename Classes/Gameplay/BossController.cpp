//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/BossController.h"
#include "Precompiled.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "Gameplay/Item.h"
#include "Boss.h"
#include "Engine/AudioManager.h"

namespace Echo
{

    BossController::BossController()
        : m_gameMapController(NULL)
        , m_boss(NULL)
        , m_bossPosX(0)
        , m_bossPosY(0)
        , m_passoutTime(0.0f)
        , m_passoutRealTime(0.0f)
        , m_lockOnTime(0.0f)
        , m_lockOnRealTime(0.0f)
        , m_moving(false)
        , m_bossDirection(eMD_Invalid)
        , m_timer(0.0f)
        , m_bossStatus(eBRS_Invalid)
    {}

    BossController::~BossController()
    {}

    bool BossController::init(Boss* boss)
    {
        m_boss = boss;
        m_bossStatus = eBRS_Normal;
        setLockOnTime(2.0f);
        setPassoutTime(2.0f);
        //m_boss->setOwner(this);
        return false;
    }

    void BossController::update(f32 dt)
    {
//         if ((m_boss->getBossGodTime() == 0) && (m_heroGodTime == 0))
//         {
//             m_boss->setBossGodTime(MetaDataHelper::getInstance()->getGodTime());
//             setHeroGodTime(MetaDataHelper::getInstance()->getGodTime());
//             if (m_boss->getOwner()->getBossStatus() == eBS_Passout)
//             {
//                 s32 HP = m_boss->getHP() - MetaDataHelper::getInstance()->getHeroDamage();
//                 ECHO_TRACE("Boss", "The boss have %d HP left !", HP);
//                 if (HP <= 0)
//                 {
//                     m_boss->getOwner()->killBoss();
//                     gameOver(true);
//                 }
//                 else
//                 {
//                     m_boss->setHP(HP);
//                 }
//             }
//             else
//             {
//                 m_curHp -= m_boss->getBossDamage();
//                 if (m_curHp <= 0)
//                 {
//                     ECHO_TRACE_NOARG("Boss", "Hero die !");
//                 }
//             }
//         }
//         else
//         {
//             f32 bossGodTime = m_boss->getBossGodTime() - dt;
//             f32 heroGodTime = m_heroGodTime - dt;
//             if (bossGodTime < 0)
//             {
//                 m_boss->setBossGodTime(0.0f);
//             }
//             else
//             {
//                 m_boss->setBossGodTime(bossGodTime);
//             }
// 
//             if (heroGodTime < 0)
//             {
//                 setHeroGodTime(0.0f);
//             }
//             else
//             {
//                 setHeroGodTime(heroGodTime);
//             }
//         }

        m_moving = m_boss->getMoving();
        m_bossPosX = m_boss->getPosX();
        m_bossPosY = m_boss->getPosY();
        switch (m_boss->getDirection())
        {
        case 0:
            m_bossDirection = eMD_Top;
            break;
        case 1:
            m_bossDirection = eMD_Left;
            break;
        case 2:
            m_bossDirection = eMD_Bottom;
            break;
        case 3:
            m_bossDirection = eMD_Right;
            break;
        default:
            break;
        }
        if (!brainSelector(dt))
        {
            ECHO_TRACE_NOARG("Boss", "You seem to be dead...");
        }
    }

    void BossController::setGameMapController(GameMapController* controller)
    {
        m_gameMapController = controller;
    }

    void BossController::setBossDirection(EMoveDirection dir)
    {
        ECHO_ASSERT(dir != NULL, "The setting direction is NULL !");
        m_bossDirection = dir;
    }

    bool BossController::brain_Normal()
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

    u32 BossController::brain_Dash()
    {
        if (m_gameMapController->getHeroCurPosX() == m_bossPosX && m_gameMapController->getHeroCurPosY() == m_bossPosY)
        {
            return 1;// hit hero
        }
        else
        {
            switch (m_bossDirection)
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


    bool BossController::judgeBlocked()
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

    void BossController::setPassoutTime(f32 pTime)
    {
        ECHO_ASSERT(pTime > 0, "The BOSS's passout time is wrong !");
        m_passoutTime = pTime;
    }

    void BossController::setLockOnTime(f32 lTime)
    {
        ECHO_ASSERT(lTime >= 0, "The BOSS's lock on time is wrong !");
        m_lockOnTime = lTime;
    }

    bool BossController::brainSelector(f32 dt)
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
            else if (midStatus == 1 )// hit hero
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

    Echo::EBossRusherStatus BossController::getBossStatus() const
    {
        return m_bossStatus;
    }
    
}