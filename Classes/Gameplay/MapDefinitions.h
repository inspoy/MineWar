//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_MAPDEFINITION_H__INCLUDED
#define ECHO_GAMEPLAY_MAPDEFINITION_H__INCLUDED

#include "Precompiled.h"

namespace Echo
{

    class Entity;

    enum EMoveDirection
    {
        eMD_Top,
        eMD_Left,
        eMD_Bottom,
        eMD_Right,

        eMD_Invalid
    };
    
    struct CubeInfo
    {
        // from left to right
        u32 m_x;

        // from bottom to top
        u32 m_y;

        // a 0~15 mask
        // init with 16
        u8 m_statusCube;
    };
    
    struct MapPoint
    {
        MapPoint();
        MapPoint(u32 x, u32 y);

        cocos2d::Vec2 toVec2();
        bool isZero();
        bool operator==(MapPoint other);
        MapPoint moveWithDirection(EMoveDirection direction);

        u32 m_x;
        u32 m_y;
        String m_goalDirection;
    };

    struct MapInfo
    {
        MapInfo();

        cocos2d::TMXTiledMap* m_tmxMap;
        MapPoint m_goalPos;
        MapPoint m_spawnPos;
        u32 m_id;
    };

}

#endif