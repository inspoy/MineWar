//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/BaseBossController.h"
#include "Precompiled.h"
#include "Boss.h"
#include "Engine/AudioManager.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "Bosses/BossRusher.h"
#include "Bosses/BossBoomer.h"
#include "Bosses/BossFlyer.h"
#include "Bosses/BossHider.h"
#include "Bosses/BossShooter.h"

namespace Echo
{

    BaseBossController::BaseBossController()
        : m_gameMapController(NULL)
        , m_boss(NULL)
        , m_bossPosX(0)
        , m_bossPosY(0)
    {}

    BaseBossController::~BaseBossController()
    {}

    bool BaseBossController::init(Boss* boss)
    {
        m_boss = boss;
        m_boss->setOwner(this);
        return true;
    }

    bool BaseBossController::uninit()
    {
        return true;
    }

    void BaseBossController::setGameMapController(GameMapController* controller)
    {
        m_gameMapController = controller;
    }

    void BaseBossController::killBoss()
    {

        auto bossSprite = m_boss->getSprite();
        AudioManager::getInstance()->playSE("enemy_die");
        bossSprite->runAction(cocos2d::Sequence::create(
            cocos2d::Spawn::create(
            cocos2d::RotateBy::create(1.0f, 720.0f),
            cocos2d::ScaleTo::create(1.0f, 0.0f),
            NULL
            ),
            cocos2d::CallFunc::create([=]()
        {
            m_boss->collect();
            if ((m_gameMapController->getBossCount() == 1) && (m_gameMapController->getOwner()->getCurMapInfo().m_id == 3))
            {
                m_gameMapController->gameOver(true);
            }
            m_gameMapController->removeBoss(this);
        }),
            NULL));

    }

    BaseBossController* BaseBossController::create(String bossName)
    {
        BaseBossController* ret = NULL;
        if (bossName == "Boss_Rusher")
        {
            auto bossRusher = new BossRusher();
            ret = dynamic_cast<BaseBossController*>(bossRusher);
        }
        else if (bossName == "Boss_Boomer")
        {
            auto bossBoomer = new BossBoomer();
            ret = dynamic_cast<BaseBossController*>(bossBoomer);
        }
        else if (bossName == "Boss_Flyer")
        {
            auto bossFlyer = new BossFlyer();
            ret = dynamic_cast<BaseBossController*>(bossFlyer);
        }
        else if (bossName == "Boss_Hider")
        {
            auto bossHider = new BossHider();
            ret = dynamic_cast<BaseBossController*>(bossHider);
        }
        else if (bossName == "Boss_Shooter")
        {
            auto bossShooter = new BossShooter();
            ret = dynamic_cast<BaseBossController*>(bossShooter);
        }
        else
        {
            ECHO_ASSERT(false, "Unknown boss name.");
        }

        return ret;
    }

    void BaseBossController::update(f32 dt)
    {
        ECHO_UNUSED(dt);
        m_bossPosX = m_boss->getPosX();
        m_bossPosY = m_boss->getPosY();
    }

    // return empty 
    Echo::Path BaseBossController::pathFinding()
    {
//         Path res;
//         res.push_back(eMD_Bottom);
//         return res;

        // create map
        u32 heroPosX = m_gameMapController->getHeroCurPosX();
        u32 heroPosY = m_gameMapController->getHeroCurPosY();
        static const u32 width = 8;
        static const u32 height = 14;
        static const s8 direction[4][2] = { { 0, 1, }, { -1, 0 }, { 0, -1 }, { 1, 0 } };
        bool map[width + 2][height + 2];
        memset(map, true, sizeof(map) / sizeof(bool));
        for (u32 i = 1; i <= width; ++i)
        {
            for (u32 j = 1; j <= height; ++j)
            {
                auto ent = m_gameMapController->getOwner()->getEntity(i, j);
                map[i][j] = ent ? ent->getBlock() : false;
            }
        }

        // A* algorithm
        std::vector<PathNode> open;
        std::vector<PathNode> closed;
        PathNode s;
        s.x = m_bossPosX;
        s.y = m_bossPosY;
        s.g = 0;
        s.h = abs(s32(m_bossPosX - heroPosX)) + abs(s32(m_bossPosY - heroPosY));
        s.f = s.h;
        open.push_back(s);

        bool found = false;
        while (!open.empty())
        {
            auto n = open.front();
            if (n.x == heroPosX && n.y == heroPosY)
            {
                found = true;
                break;
            }

            for (u32 i = 0; i < 4; ++i)
            {
                PathNode x;
                x.x = n.x + direction[i][0];
                x.y = n.y + direction[i][1];
                if (map[x.x][x.y])
                {
                    continue;
                }
                x.g = n.g + 1;
                x.h = abs(s32(x.x - heroPosX)) + abs(s32(x.y - heroPosY));
                x.f = x.g + x.h;
                x.path = n.path;
                x.path.push_back(EMoveDirection(i));

                bool xInClosed = false;
                for (auto o : closed)
                {
                    if (o.x == x.x && o.y == x.y)
                    {
                        xInClosed = true;
                        break;
                    }
                }
                if (xInClosed) continue;

                bool xInOpen = false;
                for (auto o : open)
                {
                    if (o.x == x.x && o.y == x.y)
                    {
                        xInOpen = true;
                        break;
                    }
                }
                if (xInOpen)
                {
                    if (x.f < n.f)
                    {
                        n.f = x.f;
                        open.erase(open.begin());
                        open.push_back(n);
                    }
                }
                else
                {
                    open.push_back(x);
                }
            } // end for
            closed.push_back(n);
            open.erase(open.begin());
            std::sort(open.begin(), open.end());
        } // end while

        return found ? open.front().path : Path();
    }

}