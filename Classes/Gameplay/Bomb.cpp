//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Bomb.h"
#include "Precompiled.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "base/ccMacros.h"
#include "Engine/AudioManager.h"

namespace Echo
{

    const u32 Bomb::BOMB_DAMAGE = 20;

    Bomb::Bomb()
        : m_status(eBS_Idle)
        , m_curCooldown(0.0f)
        , m_touching(false)
        , m_curLoopTime(0.0f)
        , m_loopTime(1.0f)
        , m_curDirection(eMD_Invalid)
        , m_curRound(0)
        , m_round(3)
        , m_randomTime(0.0f)
        , m_restTime(1.5f)
        , m_curRestTime(0.0f)
        , m_playerOperated(0)
        , m_dmgCount(0)
    {}

    Bomb::~Bomb()
    {}

    void Bomb::init()
    {
        
    }

    void Bomb::update(f32 dt)
    {
        if (m_status == eBS_Idle) return;

        if (m_status == eBS_InCooldown)
        {
            if (m_gameMapController->getInMiniGame() == false)
            {
                m_curCooldown += dt;
                if (m_curCooldown > m_cooldown)
                {
                    m_curCooldown = 0;
                    m_status = eBS_Idle;
                    m_sprite->setTexture("entities/Bomb_Basic.png");
                }
            }
        }
        else if (m_status == eBS_InQte)
        {
            // play QTE game
            ECHO_TRACE("Bomb", "%d", m_dmgCount);
            if (m_curRound < m_round)
            {
                m_randomTime = CCRANDOM_0_1();
                m_curRound++;
                m_status = eBS_InSelect;
            }
            else
            {
                m_curRound = 0;
                m_gameMapController->setInMiniGame(false);
                m_gameMapController->getUIController()->playUIAnimation("BombPanel_Close");
                if (m_dmgCount != 0)
                {
                    AudioManager::getInstance()->playSE("minigame_Failed");
                    explode();
                }
                else
                {
                    AudioManager::getInstance()->playSE("minigame_Clear");
                    m_sprite->setTexture("entities/Bomb_Disarmed.png");
                    m_status = eBS_InCooldown;
                    auto bgNode = dynamic_cast<cocos2d::Node*>(m_gameMapController->getUIController()->getWidgetByName("IMG_BombPanel"));
                    cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(bgNode);
                }
            }
        }
        else if (m_status == eBS_InSelect)
        {
            m_curLoopTime += dt;
            if (m_curLoopTime >= m_loopTime || m_curLoopTime >= m_randomTime)
            {
                m_curLoopTime = 2*m_loopTime - m_curLoopTime;
                u32 dir = rand() % 3;
                ECHO_TRACE("BombDir", "%d", dir);
                switch (dir)
                {
                case 0:
                    m_curDirection = eMD_Top;
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Up_Begin");
                    break;
                case 1:
                    m_curDirection = eMD_Left;
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Left_Begin");
                    break;
                case 2:
                    m_curDirection = eMD_Bottom;
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Down_Begin");
                    break;
                case 3:
                    m_curDirection = eMD_Right;
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Right_Begin");
                    break;
                default:
                    break;
                }
                m_status = eBS_InPlayerAct;
            }
        }
        else if (m_status == eBS_InPlayerAct)
        {
            m_curLoopTime -= dt;
            if (m_curLoopTime > 0.0f && m_playerOperated == 1)
            {
                switch (m_curDirection)
                {
                case Echo::eMD_Top:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Up_End_Win");
                    break;
                case Echo::eMD_Left:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Left_End_Win");
                    break;
                case Echo::eMD_Bottom:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Down_End_Win");
                    break;
                case Echo::eMD_Right:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Right_End_Win");
                    break;
                default:
                    break;
                }

                m_playerOperated = 0;
                m_curLoopTime = 0.0f;
                m_curDirection = eMD_Invalid;
                m_status = eBS_InRest;
            }
            else if (m_curLoopTime <= 0.0f || m_playerOperated == 2)
            {
                switch (m_curDirection)
                {
                case Echo::eMD_Top:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Up_End_Lose");
                    break;
                case Echo::eMD_Left:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Left_End_Lose");
                    break;
                case Echo::eMD_Bottom:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Down_End_Lose");
                    break;
                case Echo::eMD_Right:
                    m_gameMapController->getUIController()->playUIAnimation("Bomb_Right_End_Lose");
                    break;
                default:
                    break;
                }

                m_playerOperated = 0;
                m_curLoopTime = 0.0f;
                m_dmgCount++;
                m_curDirection = eMD_Invalid;
                m_status = eBS_InRest;
            }
        }
        else if (m_status == eBS_InRest)
        {
            m_curRestTime += dt;
            if (m_curRestTime >= m_restTime)
            {
                m_curRestTime = 0.0f;
                m_status = eBS_InQte;
            }
        }
    }

    void Bomb::startQte()
    {
        ECHO_ASSERT(m_cooldown > MATH_EPSILON, "Bomb cooldown has not been set.");

        if (m_status == eBS_InCooldown)
        {
            ECHO_TRACE_NOARG("Bomb","Is Cooling down !");
            return;
        }

        m_touchListener = cocos2d::EventListenerTouchOneByOne::create();
        m_touchListener->onTouchBegan = CC_CALLBACK_2(Bomb::onTouchBegan, this);
        m_touchListener->onTouchMoved = CC_CALLBACK_2(Bomb::onTouchMoved, this);
        m_touchListener->onTouchEnded = CC_CALLBACK_2(Bomb::onTouchEnded, this);
        auto bgNode = dynamic_cast<cocos2d::Node*>(m_gameMapController->getUIController()->getWidgetByName("IMG_BombPanel"));
        cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_touchListener, bgNode);

        m_status = eBS_InQte;

        m_curCooldown = 0;

        m_gameMapController->setInMiniGame(true);

        m_gameMapController->getUIController()->playUIAnimation("BombPanel_Show");
    }

    void Bomb::setCooldown(f32 val)
    {
        ECHO_ASSERT(val > 0.0f, "Illegal val.");
        m_cooldown = val;
    }

    void Bomb::explode()
    {
        ECHO_TRACE_NOARG("Bomb", "Expoled...");

        bool heroInRange = false;
        static const s32 deltaXY[2][5] = { {0, 0, -1, 0, 1 }, {0, 1, 0, -1, 0 } };
        for (u32 i = 0; i < 5; ++i)
        {
            u32 x = getPosX() + deltaXY[0][i];
            u32 y = getPosY() + deltaXY[1][i];
            // m_gameMapController->clearPos(x, y);
            auto ent = m_gameMapController->getOwner()->getEntity(x, y);
            if (ent && (ent->getType() == eET_Terrain || ent->getType() == eET_Door))
            {
                m_gameMapController->getOwner()->removeEntity(ent->getUid());
            }
            if (m_gameMapController->checkHeroReach(x, y))
            {
                heroInRange = true;
            }
        }
        if (heroInRange) ++m_dmgCount;

        m_gameMapController->reduceHp(m_dmgCount * BOMB_DAMAGE);

        auto bgNode = dynamic_cast<cocos2d::Node*>(m_gameMapController->getUIController()->getWidgetByName("IMG_BombPanel"));
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(bgNode);

        // play animation
        m_gameMapController->getOwner()->playAnimation("Bomb_Explode", getPos());
        AudioManager::getInstance()->playSE("boom");

        this->collect();
    }

    bool Bomb::onTouchBegan(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {
        return true;
    }

    void Bomb::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* /*event*/)
    {
        if (m_touching) return;

        f32 dx = touch->getLocation().x - touch->getStartLocation().x;
        f32 dy = touch->getLocation().y - touch->getStartLocation().y;

        f32 dis = dx * dx + dy * dy;

        if (dis > GameMapController::MOVE_THEREHOLD)
        {
            m_touching = true;
            EMoveDirection dir = eMD_Invalid;
            if (abs(dx) > abs(dy))
            {
                if (dx > 0)
                {
                    dir = eMD_Right;
                }
                else
                {
                    dir = eMD_Left;
                }
            }
            else
            {
                if (dy > 0)
                {
                    dir = eMD_Top;
                }
                else
                {
                    dir = eMD_Bottom;
                }
            }
            if (dir != m_curDirection)
            {
                m_playerOperated = 2;
            }
            else
            {
                m_playerOperated = 1;
            }
        }
    }

    void Bomb::onTouchEnded(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {
        m_touching = false;
    }

}