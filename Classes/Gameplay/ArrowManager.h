//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_ARROWMANAGER_H__INCLUDED
#define ECHO_GAMEPLAY_ARROWMANAGER_H__INCLUDED

#include "Gameplay/Arrow.h"
#include "Precompiled.h"
#include "Scenes/SceneGameMap.h"
#include "Trap.h"

namespace Echo
{

    class GameMapController;

    class ArrowManager
    {
    public:
        ArrowManager();
        ~ArrowManager();

        void setController(GameMapController* controller);
        void update(f32 dt);
        void initTraps(std::vector<Entity*> allEntity);
        void addArrow(Entity* trap);

    private:
        void collectArrow(Entity* arr);
        void moveArrow();

    private:
        std::vector<Arrow*> m_arrows;
        std::vector<Trap*> m_traps;
        GameMapController* m_gameMapController;
    };
}

#endif