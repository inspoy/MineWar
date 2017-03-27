//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Item.h"
#include "Precompiled.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Scenes/SceneGameMap.h"
#include "Trap.h"
#include "Boss.h"
#include "KeyAndDoor.h"
#include "Treasure.h"
#include "Bomb.h"
#include "MyBomb.h"

namespace Echo
{

    Entity::Entity()
        : m_type(eET_Invalid)
        , m_block(false)
        , m_hide(true)
        , m_pos(0, 0)
        , m_direction(eED_None)
        , m_found(false)
        , m_sprite(NULL)
        , m_gameMapController(NULL)
        , m_added(false)
    {}

    Entity::~Entity()
    {}

    void Entity::update(f32 dt)
    {
        ECHO_UNUSED(dt);
    }

    Echo::EEntityType Entity::getType() const
    {
        return m_type;
    }

    void Entity::setType(EEntityType val)
    {
        m_type = val;
    }

    cocos2d::Sprite* Entity::getSprite()
    {
        return m_sprite;
    }

    Echo::MapPoint Entity::getPos() const
    {
        return m_pos;
    }

    Echo::u32 Entity::getPosX() const
    {
        return m_pos.m_x;
    }

    Echo::u32 Entity::getPosY() const
    {
        return m_pos.m_y;
    }

    void Entity::setPos(MapPoint pos)
    {
        ECHO_ASSERT(pos.m_x > 0 && pos.m_y > 0, "Invalid pos(%d, %d).", pos.m_x, pos.m_y);

        m_pos = pos;

        updateSpritePos();
    }

    void Entity::setPos(u32 x, u32 y)
    {
        ECHO_ASSERT(x > 0 && y > 0, "Invalid pos(%d, %d).", x, y);

        m_pos.m_x = x;
        m_pos.m_y = y;

        updateSpritePos();
    }

    void Entity::setPosX(u32 x)
    {
        ECHO_ASSERT(x > 0, "Invalid pos(%d, %d).", x, m_pos.m_y);

        m_pos.m_x = x;

        updateSpritePos();
    }

    void Entity::setPosY(u32 y)
    {
        ECHO_ASSERT(y > 0, "Invalid pos(%d, %d).", m_pos.m_x, y);

        m_pos.m_y = y;

        updateSpritePos();
    }

    void Entity::setDirection(u32 dir)
    {
        ECHO_ASSERT((dir >= 0) && (dir <= 3), "Direction Num. is wrong!");

        m_direction = (EEntityDirection)dir;
        if (m_type == eET_Boss)
        {
            if (dir == eED_Left)
            {
                getSprite()->setScaleX(1.0f);
            }
            else if (dir == eED_Right)
            {
                getSprite()->setScaleX(-1.0f);
            }
        }
        else
        {
            u32 spriteDir = 0;

            switch (m_direction)
            {
            case Echo::eED_Top:
                spriteDir = 0;
                break;
            case Echo::eED_Left:
                spriteDir = 270;
                break;
            case Echo::eED_Bottom:
                spriteDir = 180;
                break;
            case Echo::eED_Right:
                spriteDir = 90;
                break;
            default:
                break;
            }
            cocos2d::Sprite *sp = Entity::getSprite();
            sp->setRotation(spriteDir);
        }
    }

    u32 Entity::getDirection() const
    {
        return (u32)m_direction;
    }

    Entity* Entity::create(String id, EEntityType eType)
    {
        Entity* ret = NULL;
        switch (eType)
        {
        case Echo::eET_Supply:
        case Echo::eET_Coin:
        case Echo::eET_Terrain:
            ret = new Entity();
            break;

        case Echo::eET_Trap:
            ret = new Trap();
            break;

        case eET_Boss:
            ret = new Boss();
            break;

        case Echo::eET_Key:
        case Echo::eET_Door:
            ret = new KeyAndDoor();
            break;

        case Echo::eET_TreasureBox:
            ret = new TreasureBox();
            break;

        case Echo::eET_Bomb:
            ret = new Bomb();
            break;

        case Echo::eET_MyBomb:
            ret = new MyBomb();
            break;

        default:
            ECHO_ASSERT(false, "Type Invalid : %d", u32(eType));
            break;
        }
        ret->m_sprite = cocos2d::Sprite::create(ECHO_STR("entities/%s.png", id.c_str()));
        ECHO_ASSERT(ret->m_sprite != NULL, "Invalid item id: %s", id.c_str());
        ret->m_uid = GameLogic::getInstance()->getNewUid();
        return ret;
    }

    void Entity::collect()
    {
        //m_sprite->release();
        auto owner = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
        ECHO_ASSERT(owner, "Could not get the SceneGameMap.");
        owner->removeEntity(m_uid);
        delete this;
    }

    Echo::u32 Entity::getUid() const
    {
        return m_uid;
    }

    bool Entity::getHide() const
    {
        return m_hide;
    }

    void Entity::setHide(bool val)
    {
        m_hide = val;

        if (val)
        {
            // add mask to make it dark
        }
        else
        {
            if (m_found)
            {
                // remove mask
            }
            else
            {
                m_sprite->setZOrder(200);
            }
        }
    }

    void Entity::updateSpritePos()
    {
        m_sprite->setPosition(
            SceneGameMap::MAP_OFFSET_X + GameMapController::SIZE_PER_BLOCK * m_pos.m_x,
            SceneGameMap::MAP_OFFSET_Y + GameMapController::SIZE_PER_BLOCK * m_pos.m_y
            );
    }

    bool Entity::getBlock() const
    {
#ifdef ECHO_CHEAT
        if (m_type == eET_Terrain)
            return false;
#endif
        return m_block;
    }

    void Entity::setBlock(bool val)
    {
        m_block = val;
    }

    void Entity::setAdded()
    {
        ECHO_ASSERT(!m_added, "Already added!");
        m_added = true;
    }

    Echo::f32 Entity::getAmount() const
    {
        return m_amount;
    }

    void Entity::setAmount(Echo::f32 val)
    {
        m_amount = val;
    }

    bool Entity::getFound() const
    {
        return m_found;
    }

    void Entity::setFound(bool val)
    {
        if (val)
        {
            setHide(false);
            m_found = true;
        }
        else
        {
            if (m_added)
            {
                ECHO_ASSERT(false, "parameter mustnot be false");
            }
            m_found = false;
        }
    }

    void Entity::setGameMapController(GameMapController* owner)
    {
        ECHO_ASSERT(owner != NULL, "Owner is NULL");
        ECHO_ASSERT(m_gameMapController == NULL, "GameMapController has already been set!");
        m_gameMapController = owner;
    }

    Echo::String Entity::typeToString(EEntityType val)
    {
        static const char* strLib[] =
        { "Supply", "Coin", "Key", "Terrain", "Trap", "Arrow", "Boss", "Door", "TreasureBox", "Bomb", "MyBomb" };

        return String(strLib[val]);
    }

}