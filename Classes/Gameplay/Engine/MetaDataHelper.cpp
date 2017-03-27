//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/MetaDataHelper.h"

namespace Echo
{
    
    ECHO_IMPLEMENT_SINGLETON(MetaDataHelper);

    MetaDataHelper::MetaDataHelper()
    {}

    MetaDataHelper::~MetaDataHelper()
    {}

    void MetaDataHelper::readMetaData()
    {
        readEntitiesMeta();
        readSaveData();
        readOtherData();
        // more meta...
    }

    Echo::EntityMetaInfo MetaDataHelper::getEntityInfo(String eId)
    {
        EntityMetaInfo ret;
        for (auto ent : m_entities)
        {
            if (eId == ent.m_fileName)
            {
                ret = ent;
                break;
            }
        }
        return ret;
    }

    void MetaDataHelper::readEntitiesMeta()
    {
        // Read Entities Meta
        ConfigData entCd("meta/Entities");
        u32 entCount = entCd.getValueWithKey("EntitiesCount").toU32();
        for (u32 i = 0; i < entCount; ++i)
        {
            EntityMetaInfo newInfo;
            // required
            // file name
            newInfo.m_fileName = entCd.getValueWithKey(ECHO_STR("Entity%d/Name", i));
            // type
            newInfo.m_type = EEntityType(entCd.getValueWithKey(ECHO_STR("Entity%d/Type", i)).toU32());
            // block
            newInfo.m_block = entCd.getValueWithKey(ECHO_STR("Entity%d/Block", i)).toU32() == 1;

            // optional
            // hide
            if (entCd.isKeyExist(ECHO_STR("Entity%d/Hide", i)))
            {
                newInfo.m_hide = entCd.getValueWithKey(ECHO_STR("Entity%d/Hide", i)).toU32() == 1;
            }
            // speed
            if (entCd.isKeyExist(ECHO_STR("Entity%d/Speed", i)))
            {
                newInfo.m_speed = entCd.getValueWithKey(ECHO_STR("Entity%d/Speed", i)).toF32();
            }
            // amount
            if (entCd.isKeyExist(ECHO_STR("Entity%d/Amount", i)))
            {
                newInfo.m_amount = entCd.getValueWithKey(ECHO_STR("Entity%d/Amount", i)).toF32();
            }
            // CD
            if (entCd.isKeyExist(ECHO_STR("Entity%d/CD", i)))
            {
                newInfo.m_cooldown = entCd.getValueWithKey(ECHO_STR("Entity%d/CD", i)).toF32();
            }

            m_entities.push_back(newInfo);
        }
    }

    void MetaDataHelper::readSaveData()
    {
        ConfigData cd("userdata/save");

    }

    void MetaDataHelper::readOtherData()
    {
        ConfigData cd("meta/meta");
        m_heroDamage = cd.getValueWithKey("HeroDamage").toU32();
        m_godTime = cd.getValueWithKey("GodTime").toF32();
    }

}