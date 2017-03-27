//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/GameSceneFactory.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Gameplay/Engine/GameObject/GameScene.h"
#include "cocostudio/CCSSceneReader.h"
#include "cocostudio/CCComRender.h"
// start scenes file
#include "Scenes/SceneGameMap.h"
#include "Scenes/SceneTitle.h"
#include "Scenes/SceneLoadMap.h"
// end

namespace Echo
{

    ECHO_IMPLEMENT_SINGLETON(GameSceneFactory);

    const u32 GameSceneFactory::UI_TAG = 9999;

    GameSceneFactory::GameSceneFactory()
    {}

    GameSceneFactory::~GameSceneFactory()
    {}

    GameScene* GameSceneFactory::createScene(String sceneName)
    {
        GameScene* ret = NULL;
        // create scene object
        if (sceneName == "SceneGameMap")
        {
            auto scene = new SceneGameMap();
            scene->init();
            ret = dynamic_cast<GameScene*>(scene);
        }
        else if (sceneName == "SceneLoadMap")
        {
            auto scene = new SceneLoadMap();
            scene->init();
            ret = dynamic_cast<GameScene*>(scene);
        }
        else if (sceneName == "SceneTitle")
        {
            auto scene = new SceneTitle();
            scene->init();
            ret = dynamic_cast<GameScene*>(scene);
        }
        else
        {
            ECHO_ASSERT(false, "Unknown scene: %s!", sceneName.c_str());
        }

        // read form json
        auto sceneReader = cocostudio::SceneReader::getInstance();
        ECHO_TRACE_NOARG("Flow", "Loading json file...");
        String sceneFileName;
        sceneFileName = ECHO_STR("publish/%s.json", sceneName.c_str());

        auto rootSceneNode = sceneReader->createNodeWithSceneFile(sceneFileName.stdStr);
        ECHO_ASSERT(rootSceneNode, "Loading failed, sceneFileName is %s", sceneFileName.c_str());
        ECHO_TRACE_NOARG("Flow", "Loading completed.");
        cocos2d::Node* uiNode = rootSceneNode->getChildByTag(UI_TAG);
        ECHO_ASSERT(uiNode, "Cannot Find UI Node From %s", sceneName.c_str());
        cocostudio::ComRender* guiRender = (cocostudio::ComRender*)(uiNode->getComponent("GUIComponent"));
        ret->setUiLayer((cocos2d::Layer*)guiRender->getNode());
        ret->setSceneLayer((cocos2d::Layer*)rootSceneNode);
        ret->addChild(rootSceneNode);
        GameLogic::getInstance()->setCurrentScene(ret);
        ret->initController();

        return ret;
    }

}