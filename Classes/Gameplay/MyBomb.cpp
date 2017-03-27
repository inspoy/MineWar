//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/MyBomb.h"
#include "Precompiled.h"
#include "Engine/GameLogic.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "Bomb.h"
#include "KeyAndDoor.h"
#include "Boss.h"
#include "Engine/AudioManager.h"

Echo::MyBomb::MyBomb()
            : m_boomTime(4.0f)
            , m_curBoomTime(0.0f)
            , m_dmgPoint(20)
{}

Echo::MyBomb::~MyBomb()
{

}

void Echo::MyBomb::init()
{

}

void Echo::MyBomb::update(f32 dt)
{
    m_curBoomTime += dt;
    if (m_curBoomTime >= m_boomTime)
    {
        explode();
    }
}

void Echo::MyBomb::explode()
{
    ECHO_TRACE_NOARG("MyBomb", "Expoled...");

    static const s32 deltaXY[5][2] = { { 0, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 }, { 1, 0 } };
    for (u32 i = 0; i < 5; ++i)
    {
        u32 x = getPosX() + deltaXY[i][0];
        u32 y = getPosY() + deltaXY[i][1];
        auto ent = m_gameMapController->getOwner()->getEntity(x, y);
        if (ent)
        {
            switch (ent->getType())
            {
            case eET_Bomb
                :
            {
                auto theBomb = dynamic_cast<Bomb*>(ent);
                theBomb->explode();
                break;
            }
            case eET_Key
                :
            {
                auto theKey = dynamic_cast<KeyAndDoor*>(ent);
                theKey->open(theKey);
                break;
            }
            case eET_Boss
                :
            {
                auto theBoss = dynamic_cast<Boss*>(ent);
                s32 HP = theBoss->getHP();
                HP -= m_dmgPoint;
                if (HP <= 0)
                {
                    theBoss->getOwner()->killBoss();
                }
                else
                {
                    theBoss->setHP(HP);
                }
                break;
            }
            default:
                break;
            }
        }
    }

    // play animation
    m_gameMapController->getOwner()->playAnimation("MyBomb_Explode", getPos());
    AudioManager::getInstance()->playSE("boom");

    this->collect();
}
