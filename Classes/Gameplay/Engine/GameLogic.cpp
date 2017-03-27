//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Gameplay/Engine/GameObject/GameScene.h"
#include "Gameplay/Engine/GameSceneFactory.h"
#include "Gameplay/Engine/BaseGameController.h"
#include "cocostudio/CCActionManagerEx.h"

namespace Echo
{

    ECHO_IMPLEMENT_SINGLETON(GameLogic);

    GameLogic::GameLogic()
        : m_director(NULL)
        , m_currentScene(NULL)
        , m_uidCount(100000)
    {}

    GameLogic::~GameLogic()
    {}

    bool GameLogic::init()
    {
        m_curLevelName = "";

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        auto keyListener = cocos2d::EventListenerKeyboard::create();
        keyListener->onKeyPressed = CC_CALLBACK_2(GameLogic::onKeyPressed, this);
        keyListener->onKeyReleased = CC_CALLBACK_2(GameLogic::onKeyReleased, this);
        getDirector()->getEventDispatcher()->addEventListenerWithFixedPriority(keyListener, 1);
        m_shotCount = 0;
        while (1)
        {
            String filename = ECHO_STR("ScreenShot_%d.png", m_shotCount);
            StdString fullPath = cocos2d::FileUtils::getInstance()->getWritablePath() + filename.stdStr;
            ECHO_BREAK_UNLESS(cocos2d::FileUtils::getInstance()->isFileExist(fullPath));
            ++m_shotCount;
        }
#endif

        return true;
    }

    void GameLogic::update(float dt)
    {
        ECHO_ASSERT(m_currentScene, "");
        ECHO_ASSERT(m_gameControllerMap.find(m_currentScene->getSceneName().stdStr) != m_gameControllerMap.end(), "");
        std::vector<BaseGameController*> controllerVec = m_gameControllerMap[m_currentSceneName.stdStr];
        for (BaseGameController* pController : controllerVec)
        {
            pController->update(dt);
        }
    }

    GameScene* GameLogic::switchScene(String sceneName, bool dontSwitch /*= false*/, bool noEffect /*= false*/)
    {
        ECHO_TRACE("Flow", "Scene switch start: %s", sceneName.c_str());
        // release all old animations
        cocostudio::ActionManagerEx::getInstance()->releaseActions();

        GameScene* oldScene = getCurrentScene<GameScene*>();
        if (oldScene)
        {
            oldScene->uninit();
            CC_SAFE_DELETE(oldScene);
        }

        auto ret = GameSceneFactory::getInstance()->createScene(sceneName);

        if (!dontSwitch)
        {
            if (noEffect)
            {
                getDirector()->replaceScene(ret->getC2dScene());
            }
            else
            {
                // set transition effect
                auto transitionScene = cocos2d::TransitionProgressRadialCCW::create(1.0f, ret->getC2dScene());
                getDirector()->replaceScene(transitionScene);
            }
        }

        ECHO_TRACE_NOARG("Flow", "Switching Complete!");
        return ret;
    }

    cocos2d::Director* GameLogic::getDirector()
    {
        if (!m_director)
        {
            m_director = cocos2d::Director::getInstance();
        }

        return m_director;
    }

    void GameLogic::setCurrentScene(GameScene* pScene)
    {
        m_currentScene = pScene;
        m_currentSceneName = pScene->getSceneName();
        std::vector<BaseGameController*> emptyVec;
        m_gameControllerMap[m_currentSceneName.stdStr] = emptyVec;
    }

    void GameLogic::registerGameController(BaseGameController* pController)
    {
        ECHO_ASSERT(pController, "invalid param!");

        m_gameControllerMap[m_currentSceneName.stdStr].push_back(pController);
    }

    void GameLogic::unregisterGameController(BaseGameController* pController)
    {
        auto controllerVec = m_gameControllerMap[m_currentSceneName.stdStr];
        auto itr = controllerVec.begin();
        for (; itr != controllerVec.end(); ++itr)
        {
            if (pController == (*itr))
            {
                controllerVec.erase(itr);
                break;
            }
        }
    }

    Echo::String GameLogic::getCurLevelName()
    {
        return m_curLevelName;
    }

    void GameLogic::setCurLevelName(String levelName)
    {
        m_curLevelName = levelName;
    }

    Echo::u32 GameLogic::getNewUid()
    {
        return ++m_uidCount;
    }

    void GameLogic::onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* /*event*/)
    {
        if (code == cocos2d::EventKeyboard::KeyCode::KEY_F12)
        {
            // catch screen
            auto size = getDirector()->getWinSize();
            auto texture = cocos2d::RenderTexture::create(int(size.width), int(size.height));
            texture->setPositionX(size.width / 2);
            texture->setPositionY(size.height / 2);
            texture->begin();
            getDirector()->getRunningScene()->visit();
            texture->end();
            String fileName = ECHO_STR("ScreenShot_%d.png", m_shotCount);
            StdString fullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(fileName.stdStr);
            texture->saveToFile(fullPath, cocos2d::Image::Format::PNG);
            ++m_shotCount;
            ECHO_TRACE("DEBUG", "Screenshot saved to %s", fileName.c_str());
        }
    }

    void GameLogic::onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* /*event*/)
    {
        ECHO_UNUSED(code);
    }

}
