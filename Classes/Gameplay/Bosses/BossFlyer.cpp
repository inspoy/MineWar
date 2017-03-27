//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Bosses/BossFlyer.h"
#include "Precompiled.h"
#include "cocos2d.h"
#include "Gameplay/Boss.h"
#include "Gameplay/Engine/AudioManager.h"
#include "Gameplay/BaseBossController.h"
#include "../Engine/MetaDataHelper.h"
#include "Scenes/GameMapController.h"
#include <math.h>

namespace Echo
{

    BossFlyer::BossFlyer()
        : m_bossGodTime(0.0f)
        , m_heroGodTime(0.0f)
    {}

    BossFlyer::~BossFlyer()
    {}

    bool BossFlyer::init(Boss* boss)
    {
        auto ret = super::init(boss);
        // animation
        auto bossAniamtion = cocos2d::Animation::create();
        for (u32 i = 1;; ++i)
        {
            auto idStr = String(i, 4, '0');
            String fp = ECHO_STR("animation/flyer/Flyer%s.png", idStr.c_str());
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
            AudioManager::getInstance()->playSE("flyer_move");
        }));
        m_boss->getSprite()->runAction(cocos2d::RepeatForever::create(bossAnimateWithSound));

        return ret;
    }

    bool BossFlyer::uninit()
    {
        return super::uninit();
    }

    void BossFlyer::update(f32 dt)
    {
        super::update(dt);
        s32 dx = m_gameMapController->getHeroCurPosX() - m_bossPosX;
        s32 dy = m_gameMapController->getHeroCurPosY() - m_bossPosY;
        f32 dz = sqrt(dx * dx + dy * dy);
        f32 sinVal = dy / dz;
        f32 cosVal = dx / dz;
        static f32 THRESHHOLD = sqrt(2.0) / 2.0;

        if (sinVal > THRESHHOLD)
        {
            // up
            if (m_bossPosX > 0 && m_bossPosX < GameMapController::getMapWidth())
            {
                m_boss->moveWithAnimation(eMD_Top);
            }
        }
        else if (sinVal < -THRESHHOLD)
        {
            // down
            if (m_bossPosX > 0 && m_bossPosX < GameMapController::getMapWidth())
            {
                m_boss->moveWithAnimation(eMD_Bottom);
            }
        }
        else
        {
            if (cosVal > THRESHHOLD)
            {
                // right
                if (m_bossPosY > 0 && m_bossPosY < GameMapController::getMapHeight())
                {
                    m_boss->moveWithAnimation(eMD_Right);
                }
            }
            else
            {
                // left
                if (m_bossPosY > 0 && m_bossPosY < GameMapController::getMapHeight())
                {
                    m_boss->moveWithAnimation(eMD_Left);
                }
            }
        }
    }

    void BossFlyer::collide(f32 dt)
    {
        if (m_bossGodTime == 0 && m_heroGodTime == 0)
        {
            m_bossGodTime = MetaDataHelper::getInstance()->getGodTime();
            m_heroGodTime = MetaDataHelper::getInstance()->getGodTime();
            
            m_gameMapController->reduceHp(m_boss->getDamage());
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

}