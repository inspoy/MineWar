//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_ITEM_H__INCLUDED
#define ECHO_GAMEPLAY_ITEM_H__INCLUDED

#include "Precompiled.h"
#include "MapDefinitions.h"

namespace Echo
{

    enum EEntityType
    {
        eET_Supply = 0,
        eET_Coin = 1,
        eET_Key = 2,
        eET_Terrain = 3,
        eET_Trap = 4,
        eET_Arrow = 5,
        eET_Boss = 6,
        eET_Door = 7,
        eET_TreasureBox = 8,
        eET_Bomb = 9,
        eET_MyBomb = 10,

        eET_Count,
        eET_Invalid
    };

    enum EEntityDirection
    {
        eED_Top = 0,
        eED_Left = 1,
        eED_Bottom = 2,
        eED_Right = 3,

        eED_None
    };

    class GameMapController;

    class Entity
    {
    public:
        Entity();
        ~Entity();

        virtual void update(f32 dt);

        static Entity* create(String id, EEntityType eType);
        static String typeToString(EEntityType val);

        u32 getUid() const;
        EEntityType getType() const;
        void setType(EEntityType val);
        cocos2d::Sprite* getSprite();
        MapPoint getPos() const;
        u32 getPosX() const;
        u32 getPosY() const;
        void setPos(MapPoint pos);
        void setPos(u32 x, u32 y);
        void setPosX(u32 x);
        void setPosY(u32 y);
        void setDirection(u32 dir);
        u32 getDirection() const;
        bool getHide() const;
        void setHide(bool hide);
        bool getBlock() const;
        void setBlock(bool val);
        f32 getAmount() const;
        void setAmount(f32 val);
        bool getFound() const;
        void setFound(bool val);

        void collect();

        void setAdded();
        void setGameMapController(GameMapController* owner);

        virtual void dummyFunc() {}
    protected:
        void updateSpritePos();

    protected:
        u32         m_uid;
        // type of this item
        EEntityType m_type;
        // if this item can block movement of other entities
        bool        m_block;
        // if this item is displayed only when the dirt has been dug
        bool        m_hide;
        // a number if it needs
        f32         m_amount;
        // position
        MapPoint    m_pos;
        // direction
        EEntityDirection m_direction;
        // if this item has been found
        bool        m_found;
        // the cocos2d sprite
        cocos2d::Sprite* m_sprite;

        // the owner
        GameMapController* m_gameMapController;

        // if the item object is added into the map
        bool        m_added;
    };

}

#endif