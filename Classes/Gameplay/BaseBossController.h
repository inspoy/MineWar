//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "Precompiled.h"
#include "MapDefinitions.h"

namespace Echo
{

    class GameMapController;
    class Boss;

    typedef std::vector<EMoveDirection> Path;
    struct PathNode
    {
        u32 x, y;
        u32 f, g, h;
        Path path;

        bool operator<(const PathNode& node) const
        {
            return f < node.f ? true : (f == node.f ? g > node.g : false);
        }
    };

    class BaseBossController
    {
    public:
        static BaseBossController* create(String bossName);
        typedef BaseBossController super;

        BaseBossController();
        ~BaseBossController();

        virtual bool init(Boss* boss);
        virtual bool uninit();
        virtual void update(f32 dt);
        virtual void collide(f32 dt) = 0;

        // first step from current position to player
        Path pathFinding();
        
        void killBoss();
        void setGameMapController(GameMapController* controller);
    protected:
        GameMapController* m_gameMapController;
        Boss* m_boss;

        u32 m_bossPosX;
        u32 m_bossPosY;
    };

}