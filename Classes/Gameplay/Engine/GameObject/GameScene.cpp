//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/GameObject/GameScene.h"
#include "Gameplay/Engine/BaseGameController.h"
#include "Gameplay/Engine/UIController.h"

namespace Echo
{

    GameScene::GameScene()
        : m_c2dScene(NULL)
        , m_uiLayer(NULL)
        , m_sceneLayer(NULL)
        , m_uiController(NULL)
    {}

    GameScene::~GameScene()
    {}

    bool GameScene::init()
    {
        m_c2dScene = cocos2d::Scene::create();
        return m_c2dScene != NULL;
    }

    bool GameScene::uninit()
    {
        CC_SAFE_DELETE(m_uiController);

        return true;
    }

    void GameScene::setSceneName(String sceneName)
    {
        m_sceneName = sceneName;
    }

    String GameScene::getSceneName()
    {
        return m_sceneName;
    }

    void GameScene::setUiLayer(cocos2d::Layer* uiLayer)
    {
        m_uiLayer = uiLayer;
        // create UIController
         m_uiController = new UIController();
         m_uiController->initWithLayout(dynamic_cast<cocos2d::Node*>(m_uiLayer), m_sceneName);
    }

    void GameScene::setSceneLayer(cocos2d::Layer* sceneLayer)
    {
        m_sceneLayer = sceneLayer;
    }

    void GameScene::addChild(cocos2d::Node* child)
    {
        m_c2dScene->addChild(child);
    }

    cocos2d::Scene* GameScene::getC2dScene()
    {
        return m_c2dScene;
    }

    UIController* GameScene::getUiController()
    {
        return m_uiController;
    }

    void GameScene::initController()
    {
        ECHO_ASSERT(false, "This Function should never be called!");
    }

    const u32 GameScene::ENTITY_ZORDER = 50;
    const u32 GameScene::ENTITY_ZORDER_FOUND = 200;

}