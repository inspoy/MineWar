//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/MapDefinitions.h"
#include "Gameplay/Item.h"

namespace Echo
{

    MapInfo::MapInfo()
        : m_tmxMap(NULL)
    {}


    MapPoint::MapPoint()
        : m_x(0)
        , m_y(0)
        , m_goalDirection("")
    {}

    MapPoint::MapPoint(u32 x, u32 y)
        : m_x(x)
        , m_y(y)
    {}

    cocos2d::Vec2 MapPoint::toVec2()
    {
        return cocos2d::Vec2(m_x, m_y);
    }

    bool MapPoint::isZero()
    {
        return m_x == 0 && m_y == 0;
    }

    bool MapPoint::operator==(MapPoint other)
    {
        return m_x == other.m_x && m_y == other.m_y;
    }

    Echo::MapPoint MapPoint::moveWithDirection(EMoveDirection direction)
    {
        MapPoint ret = *this;
        switch (direction)
        {
        case Echo::eMD_Top:
            ret.m_y += 1;
            break;
        case Echo::eMD_Left:
            ret.m_x -= 1;
            break;
        case Echo::eMD_Bottom:
            ret.m_y -= 1;
            break;
        case Echo::eMD_Right:
            ret.m_x += 1;
            break;
        default:
            ECHO_ASSERT(false, "invalid direction");
            break;
        }

        return ret;
    }

}