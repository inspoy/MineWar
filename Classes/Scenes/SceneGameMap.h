//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_SCENES_SCENEGAMEMAP_H_INCLUDED
#define ECHO_SCENES_SCENEGAMEMAP_H_INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/GameObject/GameScene.h"
#include "ui/UIScrollView.h"
#include "GameMapController.h"

namespace Echo
{

    class SceneGameMap : public GameScene
    {
    public:
        // overrides
        bool init();
        bool uninit();
        void initController();
        cocos2d::Node* getMapNode();
        cocos2d::Node* getFrameNode();
        cocos2d::Node* getUiNode();
        cocos2d::Node* getHeroNode();
        MapInfo& getCurMapInfo();
        void changeMap(u32 mapId);
        void addEntity(Entity* newOne);
        std::vector<Entity*> getCurEntities();
        Entity* getEntity(u32 x, u32 y);
        void removeEntity(u32 uid);
        void removeEntity(MapPoint pos);
        void initEntities();
        bool checkBlock(u32 x, u32 y);

        void playAnimation(String name, MapPoint pos);
        void playAnimation(String name, u32 x, u32 y);

    private:
        GameMapController* m_gameMapController;

        cocos2d::Node* m_heroNode;
        cocos2d::Node* m_frameNode;
        std::vector<MapInfo> m_tmxMaps;
        // entities<mapId, entities>
        std::map<u32, std::vector<Entity*> > m_entities;
        MapInfo m_curMap;

        String m_mapName;

    public:
        static const u32 MAP_ZORDER;
        static const u32 MAP_TAG;
        static const f32 MAP_OFFSET_X;
        static const f32 MAP_OFFSET_Y;
        static const u32 UI_ZORDER;
    };

}

#endif