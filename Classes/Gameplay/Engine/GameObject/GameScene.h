//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_GAMEOBJECT_GAMESCENE_H__INCLUDED
#define ECHO_GAMEPLAY_GAMEOBJECT_GAMESCENE_H__INCLUDED

#include "Precompiled.h"
#include "cocos2d.h"

namespace Echo
{

    class GameLayer;
    class UIController;

    class GameScene
    {
    public:
        typedef GameScene super;
        GameScene();
        ~GameScene();
        virtual bool init();
        virtual bool uninit();
        virtual void initController();
        void setSceneName(String sceneName);
        String getSceneName();

        cocos2d::Scene* getC2dScene();
        // method of cocos2d::Scene
        void addChild(cocos2d::Node* child);

        void setUiLayer(cocos2d::Layer* uiLayer);
        void setSceneLayer(cocos2d::Layer* sceneLayer);

        UIController* getUiController();

    protected:
        cocos2d::Scene* m_c2dScene;

        String m_sceneName;
        cocos2d::Layer* m_uiLayer;
        cocos2d::Layer* m_sceneLayer;
        UIController* m_uiController;

        static const u32 ENTITY_ZORDER;
        static const u32 ENTITY_ZORDER_FOUND;
    };

}

#endif