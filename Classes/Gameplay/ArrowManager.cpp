//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/ArrowManager.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Precompiled.h"
#include "Scenes/GameMapController.h"
#include "Engine/AudioManager.h"

namespace Echo
{
    ArrowManager::ArrowManager()
        : m_gameMapController(NULL)
    {
    }

    ArrowManager::~ArrowManager()
    {
    }

    void ArrowManager::initTraps(std::vector<Entity*> allEntity)
    {
        m_traps.clear();
        for (auto curEnt : allEntity)
        {
            if (curEnt->getType() == eET_Trap)
            {
                m_traps.push_back((Trap*)curEnt);
            }
        }

        m_arrows.clear();
    }

    void ArrowManager::addArrow(Entity* trap)
    {
        SceneGameMap* owner = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
        Arrow* ent = Arrow::create("Arrow_Basic");

        ent->setType(eET_Arrow);
        ent->setAmount(20);
        ent->setDirection(trap->getDirection());
        ent->setFound(trap->getFound());
        ent->setHide(trap->getHide());
        ent->setPos(trap->getPos());
        ent->setBlock(false);
        ent->setMoveDuration(0.2f);
        ent->setDamage(20.0f);//this only for test

        m_arrows.push_back(ent);
        owner->addEntity((Entity*)ent);

        AudioManager::getInstance()->playSE("shoot");

        ECHO_TRACE("Arrow", "Arrow Added! uid = %d", ent->getUid());
    }

    void ArrowManager::collectArrow(Entity* arr)
    {
        for (auto itr = m_arrows.begin(); itr != m_arrows.end(); ++itr)
        {
            if (*itr == arr)
            {
                itr = m_arrows.erase(itr);
                break;
            }
        }
        arr->collect();
    }

    void ArrowManager::moveArrow()
    {
        bool reach = false;

        for (auto curEnt = m_arrows.begin(); curEnt != m_arrows.end();)
        {
            reach = false;

            if (!(*curEnt)->getMoving())
            {
                switch ((*curEnt)->getDirection())
                {
                case eED_Top:
                    if ((GameMapController::getMapHeight() - 1) > (*curEnt)->getPosY())
                    {
                        auto ents = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
                        for (auto theEnt : ents->getCurEntities())
                        {
                            if (theEnt->getBlock())
                            {
                                if ((*curEnt)->getPosX() == theEnt->getPosX() && (*curEnt)->getPosY() == theEnt->getPosY() - 1)
                                {
                                    reach = true;
                                    break;
                                }
                            }
                            else if (((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX() 
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY() - 1)
                                || ((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX()
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY()))
                            {
                                reach = true;
                                m_gameMapController->reduceHp((*curEnt)->getDamage());
                                break;
                            }
                        }
                        if (!reach)
                        {
                            (*curEnt)->moveWithAnimation(eMD_Top);
                        }
                    }
                    else
                    {
                        reach = true;
                    }
                    break;
                case eED_Left:
                    if ((*curEnt)->getPosX() > 1)
                    {
                        auto ents = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
                        for (auto theEnt : ents->getCurEntities())
                        {
                            if (theEnt->getBlock())
                            {
                                if ((*curEnt)->getPosX() == theEnt->getPosX() + 1 && (*curEnt)->getPosY() == theEnt->getPosY())
                                {
                                    reach = true;
                                    break;
                                }
                            }
                            else if (((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX() + 1 
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY()) 
                                || ((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX() 
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY()))
                            {
                                reach = true;
                                m_gameMapController->reduceHp((*curEnt)->getDamage());
                                break;
                            }
                        }
                        if (!reach)
                        {
                            (*curEnt)->moveWithAnimation(eMD_Left);
                        }
                    }
                    else
                    {
                        reach = true;
                    }
                    break;
                case eED_Bottom:
                    if ((*curEnt)->getPosY() > 1)
                    {
                        auto ents = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
                        for (auto theEnt : ents->getCurEntities())
                        {
                            if (theEnt->getBlock())
                            {
                                if ((*curEnt)->getPosX() == theEnt->getPosX() && (*curEnt)->getPosY() == theEnt->getPosY() + 1)
                                {
                                    reach = true;
                                    break;
                                }
                            }
                            else if (((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX() 
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY() + 1)
                                || ((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX()
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY()))
                            {
                                reach = true;
                                m_gameMapController->reduceHp((*curEnt)->getDamage());
                                break;
                            }
                        }
                        if (!reach)
                        {
                            (*curEnt)->moveWithAnimation(eMD_Bottom);
                        }
                    }
                    else
                    {
                        reach = true;
                    }
                    break;
                case eED_Right:
                    if ((GameMapController::getMapWidth() - 1) > (*curEnt)->getPosY())
                    {
                        auto ents = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
                        for (auto theEnt : ents->getCurEntities())
                        {
                            if (theEnt->getBlock())
                            {
                                if ((*curEnt)->getPosX() == theEnt->getPosX() - 1 && (*curEnt)->getPosY() == theEnt->getPosY())
                                {
                                    reach = true;
                                    break;
                                }
                            }
                            else if (((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX() - 1 
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY())
                                || ((*curEnt)->getPosX() == m_gameMapController->getHeroCurPosX()
                                && (*curEnt)->getPosY() == m_gameMapController->getHeroCurPosY()))
                            {
                                reach = true;
                                m_gameMapController->reduceHp((*curEnt)->getDamage());
                                break;
                            }
                        }
                        if (!reach)
                        {
                            (*curEnt)->moveWithAnimation(eMD_Right);
                        }
                    }
                    else
                    {
                        reach = true;
                    }
                    break;
                default:
                    ECHO_ASSERT(false, "Unknown direction!");
                    break;
                }

                m_gameMapController->refreshSight(0, 0, true);
                if (reach)
                {
                    auto backupEnt = *curEnt;
                    curEnt = m_arrows.erase(curEnt);
                    backupEnt->collect();
                }
            }

            if (!reach)
            {
                ++curEnt;
            }
        }
    }

    void ArrowManager::update(f32 dt)
    {
        for (auto curTrap : m_traps)
        {
            if (curTrap->updateCooldown(dt))
            {
                addArrow(curTrap);
            }
        }

        moveArrow();
    }

    void ArrowManager::setController(GameMapController* controller)
    {
        m_gameMapController = controller;
    }

}
