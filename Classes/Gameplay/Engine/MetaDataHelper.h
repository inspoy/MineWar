//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_METADATAHELPER_H__INCLUDED
#define ECHO_GAMEPLAY_METADATAHELPER_H__INCLUDED

#include "Precompiled.h"
#include "Gameplay/MapDefinitions.h"
#include "Gameplay/Item.h"

namespace Echo
{

    struct EntityMetaInfo
    {
        EntityMetaInfo()
        : m_type(eET_Invalid)
        , m_block(false)
        , m_hide(false)
        , m_speed(0.0f)
        , m_amount(0.0f)
        , m_cooldown(0.0f)
        {}

        String m_fileName;
        EEntityType m_type;
        bool m_block;
        bool m_hide;
        f32 m_speed;
        f32 m_amount;
        f32 m_cooldown;
    };

    struct SaveData
    {
        u32 m_coins;
        u32 m_highScore;
    };

    class MetaDataHelper
    {
    public:
        ECHO_DECLARE_SINGLETON(MetaDataHelper);

    public:
        void readMetaData();
        
        SaveData& getSaveData() { return m_saveData; }
        EntityMetaInfo getEntityInfo(String eId);
        u32 getHeroDamage() const { return m_heroDamage; }
        f32 getGodTime() const { return m_godTime;  }

    private:
        void readEntitiesMeta();
        void readSaveData();
        void readOtherData();
        std::vector<EntityMetaInfo> m_entities;
        u32 m_heroDamage;
        f32 m_godTime;
        SaveData m_saveData;
    };

}

#endif