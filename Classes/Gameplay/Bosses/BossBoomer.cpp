//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Bosses/BossBoomer.h"
#include "Precompiled.h"
#include "../Boss.h"
#include "../Engine/GameLogic.h"
#include "Scenes/SceneGameMap.h"
#include "../Bomb.h"
#include "../Engine/AudioManager.h"

namespace Echo
{

    const f32 BossBoomer::THRESHHOLD_EXPLODE = 0.5f;

    BossBoomer::BossBoomer()
        : m_curState(eBS_Invalid)
        , m_explodeTimer(0.0f)
    {}

    BossBoomer::~BossBoomer()
    {}

    bool BossBoomer::init(Boss* boss)
    {
        auto ret = super::init(boss);
        // animation
        auto bossAniamtion = cocos2d::Animation::create();
        for (u32 i = 1;; ++i)
        {
            auto idStr = String(i, 4, '0');
            String fp = ECHO_STR("animation/boomer/boomer%s.png", idStr.c_str());
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
            AudioManager::getInstance()->playSE("boomer_move");
        }));
        m_boss->getSprite()->runAction(cocos2d::RepeatForever::create(bossAnimateWithSound));

        // FSM: init state
        m_curState = eBS_Running;

        return ret;
    }

    bool BossBoomer::uninit()
    {
        return super::uninit();
    }

    void BossBoomer::update(f32 dt)
    {
        super::update(dt);
        // FSM: update
        switch (m_curState)
        {
        case Echo::BossBoomer::eBS_Running:
            BossState_Running_Update(dt);
            break;
        case Echo::BossBoomer::eBS_Exploding:
            BossState_Exploding_Update(dt);
            break;
        case Echo::BossBoomer::eBS_Dead:
            BossState_Dead_Update(dt);
            break;
        default:
            ECHO_ASSERT(false, "Invalid boss state.");
            break;
        }
    }

    void BossBoomer::collide(f32 dt)
    {
        ECHO_UNUSED(dt);

        // FSM: state transition
        if (m_curState == eBS_Running)
        {
            m_curState = eBS_Exploding;
        }
    }

    void BossBoomer::BossState_Running_Update(f32 /*dt*/)
    {
        if (m_boss->getMoving()) return;

        auto path = pathFinding();
        if (path.size() == 0)
        {
            m_boss->moveWithAnimation(eMD_Invalid);
        }
        else
        {
            m_boss->moveWithAnimation(path.front());
        }
    }

    void BossBoomer::BossState_Exploding_Update(f32 dt)
    {
        if (m_boss->getMoving()) return;

        m_explodeTimer += dt;
        if (m_explodeTimer >= THRESHHOLD_EXPLODE)
        {
            // create a bomb and explode it
            auto bombEnt = Entity::create("Bomb_Basic", eET_Bomb);
            bombEnt->setType(eET_Bomb);
            bombEnt->setGameMapController(m_gameMapController);
            bombEnt->setPosX(m_bossPosX);
            bombEnt->setPosY(m_bossPosY);
            auto owner = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
            owner->addEntity(bombEnt);

            auto bomb = dynamic_cast<Bomb*>(bombEnt);
            ECHO_ASSERT(bomb, "");
            bomb->explode();

            killBoss();
            // FSM: state transition
            m_curState = eBS_Dead;
        }
    }

    void BossBoomer::BossState_Dead_Update(f32 /*dt*/)
    {

    }

}