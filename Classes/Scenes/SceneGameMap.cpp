//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Scenes/SceneGameMap.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Gameplay/MapDefinitions.h"
#include "Gameplay/Item.h"
#include "Gameplay/BossController.h"
#include "Gameplay/Boss.h"
#include "Gameplay/KeyAndDoor.h"
#include "Gameplay/Trap.h"
#include "Gameplay/Treasure.h"
#include "Gameplay/Bomb.h"
#include "2d/CCSprite.h"
#include "Gameplay/Engine/AudioManager.h"

namespace Echo
{

    bool SceneGameMap::init()
    {
        super::init();

        m_gameMapController = NULL;
        m_heroNode = NULL;
        m_frameNode = NULL;
        // init blank map info
        m_tmxMaps.clear();
        MapInfo mi;
        m_tmxMaps.push_back(mi);
        m_tmxMaps.push_back(mi);
        m_tmxMaps.push_back(mi);
        m_tmxMaps.push_back(mi);

        // init blank item map
        m_entities.clear();
        std::vector<Entity*> EmptyVec;
        m_entities[1] = EmptyVec;
        m_entities[2] = EmptyVec;
        m_entities[3] = EmptyVec;

        m_sceneName = "SceneGameMap";

        return true;
    }

    bool SceneGameMap::uninit()
    {
        super::uninit();
        m_gameMapController->uninit();
        CC_SAFE_DELETE(m_gameMapController);
        return true;
    }

    void SceneGameMap::initController()
    {
        m_mapName = GameLogic::getInstance()->getCurLevelName();

        // read map config file
        auto filePath = ECHO_STR("maps/%sConfig", m_mapName.c_str());
        ConfigData confData(filePath);
        for (u32 i = 1; i <= 3; ++i)
        {
            m_tmxMaps[i].m_id = i;

            // spawn position
            m_tmxMaps[i].m_spawnPos.m_x = confData.getValueWithKey(ECHO_STR("spawn%dPosX", i)).toU32();
            m_tmxMaps[i].m_spawnPos.m_y = confData.getValueWithKey(ECHO_STR("spawn%dPosY", i)).toU32();
            m_tmxMaps[i].m_spawnPos.m_goalDirection = confData.getValueWithKey(ECHO_STR("spawn%dDirection", i));

            // goal position
            m_tmxMaps[i].m_goalPos.m_x = confData.getValueWithKey(ECHO_STR("goal%dPosX", i)).toU32();
            m_tmxMaps[i].m_goalPos.m_y = confData.getValueWithKey(ECHO_STR("goal%dPosY", i)).toU32();
            m_tmxMaps[i].m_goalPos.m_goalDirection = confData.getValueWithKey(ECHO_STR("goal%dDirection", i));
        }

        m_heroNode = m_sceneLayer->getChildByName("Hero");
        ECHO_ASSERT(m_heroNode, "Cannot find the hero node.");

        m_frameNode = m_sceneLayer->getChildByName("Frame");
        ECHO_ASSERT(m_frameNode, "Cannot find the frame node.");

        for (u32 i = 1; i <= 3; ++i)
        {
            m_tmxMaps[i].m_tmxMap = cocos2d::TMXTiledMap::create(ECHO_STR("maps/%s_%d.tmx", m_mapName.c_str(), i));
            ECHO_ASSERT(m_tmxMaps[i].m_tmxMap, "Cannot create the tmxMap%d node.", i);
            m_tmxMaps[i].m_tmxMap->setPosition(MAP_OFFSET_X, MAP_OFFSET_Y);
            m_tmxMaps[i].m_tmxMap->retain();
        }
        m_sceneLayer->addChild(m_tmxMaps[1].m_tmxMap, MAP_ZORDER, MAP_TAG);
        m_curMap = m_tmxMaps[1];

        m_gameMapController = new GameMapController();
        m_gameMapController->setOwner(this);
        m_gameMapController->init("GameMapController");

        initEntities();
    }

    cocos2d::Node* SceneGameMap::getMapNode()
    {
        return dynamic_cast<cocos2d::Node*>(m_curMap.m_tmxMap);
    }

    cocos2d::Node* SceneGameMap::getHeroNode()
    {
        return m_heroNode;
    }

    MapInfo& SceneGameMap::getCurMapInfo()
    {
        return m_curMap;
    }

    void SceneGameMap::changeMap(u32 mapId)
    {
        ECHO_ASSERT(mapId > 0 && mapId < 4, "Invalid mapId!");
        // stop all SE
        AudioManager::getInstance()->stopSE();
        // remove all entities
        auto curEnts = getCurEntities();
        for (auto itr : curEnts)
        {
            itr->collect();
        }
        m_gameMapController->clearBosses();
        // remove the old map
        m_sceneLayer->removeChildByTag(MAP_TAG);
        // add the new map
        m_curMap = m_tmxMaps[mapId];
        m_sceneLayer->addChild(m_curMap.m_tmxMap, MAP_ZORDER, MAP_TAG);
        // add new entities
        initEntities();
    }

    cocos2d::Node* SceneGameMap::getFrameNode()
    {
        return m_frameNode;
    }

    void SceneGameMap::addEntity(Entity* newOne)
    {
        ECHO_ASSERT(newOne, "Entity is NULL!");
        ECHO_ASSERT(!newOne->getPos().isZero(), "The new entity %d has not been initialized!", newOne->getUid());

        newOne->setAdded();
        m_entities[m_curMap.m_id].push_back(newOne);
        m_sceneLayer->addChild(newOne->getSprite(), newOne->getFound() ? ENTITY_ZORDER_FOUND : ENTITY_ZORDER, newOne->getUid());
    }

    void SceneGameMap::removeEntity(u32 uid)
    {
        std::vector<Entity*>& vec = m_entities[m_curMap.m_id];
        for (auto itr = vec.begin(); itr != vec.end(); ++itr)
        {
            if ((*itr)->getUid() == uid)
            {
                itr = vec.erase(itr);
                break;
            }
        }
        m_sceneLayer->removeChildByTag(uid);
    }

    void SceneGameMap::removeEntity(MapPoint pos)
    {
        u32 uid = 0;
        std::vector<Entity*>& vec = m_entities[m_curMap.m_id];
        for (auto itr = vec.begin(); itr != vec.end(); ++itr)
        {
            if ((*itr)->getPos() == pos)
            {
                uid = (*itr)->getUid();
                CC_SAFE_DELETE((*itr));
                itr = vec.erase(itr);
                break;
            }
        }
        m_sceneLayer->removeChildByTag(uid);
    }

    std::vector<Entity*> SceneGameMap::getCurEntities()
    {
        return m_entities[m_curMap.m_id];
    }

    Entity* SceneGameMap::getEntity(u32 x, u32 y)
    {
        // if there are 2 or more entities in this position, return who has the smallest uid
        // if there are no entities in this position, return NULL

        Entity* ret = NULL;
        for (auto itr : m_entities[m_curMap.m_id])
        {
            if (itr->getPos() == MapPoint(x, y))
            {
                ret = itr;
                break;
            }
        }

        return ret;
    }

    cocos2d::Node* SceneGameMap::getUiNode()
    {
        return dynamic_cast<cocos2d::Node*>(m_uiLayer);
    }

    void SceneGameMap::initEntities()
    {
        String mapName = m_mapName;
        u32 mapId = m_curMap.m_id;
        ConfigData cd(ECHO_STR("maps/%sConfig", mapName.c_str()));

        u32 itemCount = cd.getValueWithKey(ECHO_STR("map%dItemCount", mapId)).toU32();
        for (u32 j = 1; j <= itemCount; ++j)
        {
            String prefix = ECHO_STR("map%dItem%d", mapId, j);

            // create
            String fileName = cd.getValueWithKey(prefix);
            EntityMetaInfo entMeta = MetaDataHelper::getInstance()->getEntityInfo(fileName);
            EEntityType itemType = entMeta.m_type;
            Entity* ent = Entity::create(fileName, itemType);
            ent->setGameMapController(m_gameMapController);

            // position
            u32 posX = cd.getValueWithKey(ECHO_STR("%sPosX", prefix.c_str())).toU32();
            u32 posY = cd.getValueWithKey(ECHO_STR("%sPosY", prefix.c_str())).toU32();
            ent->setPos(posX, posY);

            // type
            ent->setType(itemType);
            switch (itemType)
            {
            case eET_Supply
                :
            {
                // amount
                f32 amount = entMeta.m_amount;
                ent->setAmount(amount);
                break;
            }
            case eET_Trap
                :
            {
                // direction
                u32 dir = cd.getValueWithKey(ECHO_STR("%sDir", prefix.c_str())).toU32();
                ent->setDirection(dir);
                auto trap = dynamic_cast<Trap*>(ent);
                trap->setCooldown(entMeta.m_cooldown);
                break;
            }
            case eET_Boss
                :
            {
                auto boss = dynamic_cast<Boss*>(ent);
                f32 speed = entMeta.m_speed;
                boss->setMoveDuration(speed);
                BaseBossController* bossController = BaseBossController::create(entMeta.m_fileName);
                bossController->setGameMapController(m_gameMapController);
                bossController->init(boss);
                m_gameMapController->addBoss(bossController);
                u32 dir = cd.getValueWithKey(ECHO_STR("%sDir", prefix.c_str())).toU32();
                ent->setDirection(dir);
                u32 HP = cd.getValueWithKey(ECHO_STR("%sHP", prefix.c_str())).toU32();
                boss->setHP(HP);
                f32 Dam = cd.getValueWithKey(ECHO_STR("%sDam", prefix.c_str())).toF32();
                boss->setDamage(Dam);
                break;
            }
            case eET_TreasureBox
                :
            {
//                 auto box = dynamic_cast<TreasureBox*>(ent);
                break;
            }
            case eET_Bomb
                :
            {
                auto bomb = dynamic_cast<Bomb*>(ent);
                //bomb->init();
                bomb->setCooldown(entMeta.m_cooldown);
                break;
            }
            case eET_Door
                :
            {
                auto door = dynamic_cast<KeyAndDoor*>(ent);
                u32 kTag = cd.getValueWithKey(ECHO_STR("%sKTag", prefix.c_str())).toU32();
                door->setKeyTag(kTag);
                door->setOpenTime(entMeta.m_cooldown);
                String fileName = ECHO_STR("entities/Stone_%d.png", cocos2d::random() % 3 + 1);
                door->setFileName(fileName);
                ent->getSprite()->setTexture(fileName.c_str());
                ent->setFound(true);
                ent->setHide(false);
                break;
            }
            case eET_Key
                :
            {
                auto key = dynamic_cast<KeyAndDoor*>(ent);
                u32 kTag = cd.getValueWithKey(ECHO_STR("%sKTag", prefix.c_str())).toU32();
                key->setKeyTag(kTag);
                break;
            }
            case eET_Terrain
                :
            {
                String fileName = ECHO_STR("entities/Stone_%d.png", cocos2d::random() % 3 + 1);
                ent->getSprite()->setTexture(fileName.c_str());
                ent->setFound(true);
                ent->setHide(false);
            }
            default:
                break;
            }

            // block
            bool isBlock = entMeta.m_block;
            ent->setBlock(isBlock);

            addEntity(ent);

#ifdef COCOS2D_DEBUG
            if (ent->getType() != eET_Terrain)
            {
                ECHO_TRACE("Entity", "New entity added, uid=%d, type=%s", ent->getUid(), Entity::typeToString(ent->getType()).c_str());
            }
#endif
        }
    }

    void SceneGameMap::playAnimation(String name, u32 x, u32 y)
    {
        if (name == "MyBomb_Explode" || name == "Bomb_Explode")
        {
            // tween
            auto realPos = GameMapController::getRealPos(x, y);
            auto explodeSprite = cocos2d::Sprite::create("animation/explode.png");
            m_sceneLayer->addChild(explodeSprite, ENTITY_ZORDER_FOUND);
            explodeSprite->setPosition(realPos);
            explodeSprite->setScale(0.33f);
            explodeSprite->setOpacity(0);
            auto zoomInAction = cocos2d::ScaleTo::create(0.4f,1.0f);
            auto fadeInAction = cocos2d::FadeTo::create(0.2f, 255);
            auto explodeAction = cocos2d::Spawn::create(zoomInAction, fadeInAction, NULL);
            auto zoomOutAction = cocos2d::ScaleTo::create(0.2f, 0.0f);
            auto fadeOutAction = cocos2d::FadeTo::create(0.2f, 0);
            auto evanesceAction = cocos2d::Spawn::create(zoomOutAction, fadeOutAction, NULL);
            explodeSprite->runAction(cocos2d::Sequence::create(
                explodeAction,
                cocos2d::DelayTime::create(0.1f),
                evanesceAction,
                cocos2d::CallFunc::create([=]()
            {
                explodeSprite->removeFromParent();
            }),
                NULL));
        }
//         else if (name == "Bomb_Explode")
//         {
//             // tween
//         }
        else if (name == "DestroyWall")
        {
            // frame
            auto realPos = GameMapController::getRealPos(x, y);
            auto wallSprite = cocos2d::Sprite::create("animation/wall/wall0.png");
            m_sceneLayer->addChild(wallSprite, ENTITY_ZORDER_FOUND);
            wallSprite->setPosition(realPos);
            auto wallAnimation = cocos2d::Animation::create();
            u32 i = 1;
            while (1)
            {
                auto filePath = ECHO_STR("animation/wall/wall%d.png", i);
                if (cocos2d::FileUtils::getInstance()->isFileExist(filePath))
                {
                    wallAnimation->addSpriteFrameWithFile(filePath);
                    ++i;
                }
                else
                {
                    break;
                }
            }
            wallAnimation->setDelayPerUnit(0.5f / i);
            wallAnimation->setRestoreOriginalFrame(true);
            wallAnimation->setLoops(1);
            auto wallAnimate = cocos2d::Animate::create(wallAnimation);
            wallSprite->runAction(cocos2d::Sequence::createWithTwoActions(wallAnimate, cocos2d::CallFunc::create([=](){
                wallSprite->removeFromParent();
            })));
        }
        else if (name == "AddHp")
        {
            // tween
            // from x to y
            // flash one by one
            x = 0;
            for (u32 i = x + 1; i <= y; ++i)
            {
                cocos2d::Vec2 realPos(83 + i * 40, 1233);
                auto hpSprite = cocos2d::Sprite::create("animation/hp/Hp_Flash.png");
                m_sceneLayer->addChild(hpSprite, UI_ZORDER + 1);
                hpSprite->setPosition(realPos);
                hpSprite->setOpacity(0);
                hpSprite->setScale(1.5f);
                hpSprite->runAction(cocos2d::Sequence::create(
                    cocos2d::DelayTime::create((i - x - 1) * 0.1f),
                    cocos2d::FadeIn::create(0.3f),
                    cocos2d::FadeOut::create(0.2f),
                    cocos2d::CallFunc::create([=]()
                {
                    hpSprite->removeFromParent();
                }),
                    NULL
                    ));
            }
        }
        else if (name == "ReduceHp")
        {
            // tween
            // from x down to y
            // fade out one by one
            for (u32 i = x; i > y; --i)
            {
                cocos2d::Vec2 realPos(83 + i * 40, 1233);
                auto hpSprite = cocos2d::Sprite::create("animation/hp/Hp_Red.png");
                ECHO_ASSERT(hpSprite, "Load red hp failed.");
                m_sceneLayer->addChild(hpSprite, UI_ZORDER + 1);
                hpSprite->setPosition(realPos);
                hpSprite->runAction(cocos2d::Sequence::create(
                    cocos2d::FadeOut::create(0.5f),
                    cocos2d::CallFunc::create([=]()
                {
                    hpSprite->removeFromParent();
                }),
                    NULL
                    ));
            }
        }
        else
        {
            ECHO_TRACE("GameMap", "Warning: invalid animation: %s", name.c_str());
        }
    }

    void SceneGameMap::playAnimation(String name, MapPoint pos)
    {
        playAnimation(name, pos.m_x, pos.m_y);
    }

    bool SceneGameMap::checkBlock(u32 x, u32 y)
    {
        bool ret = false;

        if (x < 1 || x > GameMapController::MAP_WIDTH ||
            y < 1 || y > GameMapController::MAP_HEIGHT)
        {
            ret = true;
        }
        else
        {
            auto ent = getEntity(x, y);
            if (ent && ent->getBlock())
            {
                ret = true;
            }
        }

        return ret;
    }

    const u32 SceneGameMap::MAP_TAG = 8888;
    const u32 SceneGameMap::MAP_ZORDER = 500;
    const f32 SceneGameMap::MAP_OFFSET_X = 0.0f;
    const f32 SceneGameMap::MAP_OFFSET_Y = 0.0f;
    const u32 SceneGameMap::UI_ZORDER = 1000;

}