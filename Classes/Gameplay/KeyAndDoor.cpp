//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------


#include "Gameplay/KeyAndDoor.h"
#include "Precompiled.h"
#include "Engine/GameLogic.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "Engine/AudioManager.h"

namespace Echo
{

    void KeyAndDoor::setKeyTag(u32 kTag)
    {
        m_kTag = kTag;
    }

    Echo::u32 KeyAndDoor::getKeyTag() const
    {
        return m_kTag;
    }

    KeyAndDoor::KeyAndDoor()
    {

    }

    KeyAndDoor::~KeyAndDoor()
    {

    }

    void KeyAndDoor::update(f32 dt)
    {
        if (m_type == eET_Key)
        {

        }
        else // is door
        {
            if (!getBlock()) // door is open
            {
                m_curOpenTime -= dt;
                if (m_curOpenTime <= 0.0f) // time is up
                {
                    // close the door
                    setBlock(true);
                    m_curOpenTime = 0.0f;

//                     u32 x = m_pos.m_x;
//                     u32 y = m_pos.m_y;
//                     m_gameMapController->clearPos(x, y, true);
                    getSprite()->setTexture(m_fileName.c_str());
                    AudioManager::getInstance()->playSE("stone_recover");

                    // reset key
                    auto ents = m_gameMapController->getOwner()->getCurEntities();
                    for (auto keyEnt : ents)
                    {
                        if (keyEnt->getType() == eET_Key)
                        {
                            auto key = dynamic_cast<KeyAndDoor*>(keyEnt);
                            if (key->getKeyTag() == m_kTag)
                            {
                                key->getSprite()->setTexture("entities/Key_Basic.png");
                            }
                        }
                    }
                }
            }
        }
    }

    Echo::f32 KeyAndDoor::getCurOpenTime() const
    {
        return m_curOpenTime;
    }

    void KeyAndDoor::setCurOpenTime(Echo::f32 val)
    {
        m_curOpenTime = val;
    }

    void KeyAndDoor::resetCurOpenTime()
    {
        m_curOpenTime = m_totalOpenTime;
    }

    void KeyAndDoor::setOpenTime(f32 val)
    {
        m_totalOpenTime = val;
    }

    void KeyAndDoor::open(KeyAndDoor* theKey)
    {
        SceneGameMap* m_owner = m_gameMapController->getOwner();
        std::vector<Entity*> ents = m_owner->getCurEntities();
        for (auto doorEnt : ents)
        {
            if (doorEnt->getType() == eET_Door)
            {
                KeyAndDoor* door = dynamic_cast<KeyAndDoor*>(doorEnt);
                if (door->getKeyTag() == theKey->getKeyTag())
                {
                    ECHO_TRACE("KeyAndDoor", "Open door: %d with key: %d", door->getUid(), theKey->getUid());

                    // dig wall
                    if (door->getBlock())
                    {
                        u32 x = door->getPosX();
                        u32 y = door->getPosY();
                        m_gameMapController->getOwner()->playAnimation("DestroyWall", x, y);
                        AudioManager::getInstance()->playSE("stone_claps");
                    }
                    // if (door && door->getBlock())
                    // {
                    //     m_gameMapController->clearPos(x, y);
                    // }

                    // open door
                    door->getSprite()->setTexture("entities/Door_Close.png");
                    door->setBlock(false);
                    door->resetCurOpenTime();

                    // activate key
                    theKey->getSprite()->setTexture("entities/Key_Activated.png");
                }
            }
        }
    }

    void KeyAndDoor::setFileName(String fileName)
    {
        m_fileName = fileName;
    }

}
