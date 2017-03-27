//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Scenes/LoginListener.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Gameplay/Engine/AudioManager.h"
#include "base/ccRandom.h"
#include "ui/UIImageView.h"
#include "ui/UIButton.h"

namespace Echo
{

    const f32 LoginListener::COOLDOWN = 0.5f;
    const u32 LoginListener::MAX_LEVEL = 2;

    LoginListener::LoginListener()
        : m_owner(NULL)
        , m_curSession(eMS_Title)
        , m_curCd(0.0f)
        , m_lightTime(0.0f)
        , m_lightCd(5.0f)
        , m_curLevel(0)
        , m_started(false)
        , m_showCredit(false)
        , m_showExitPanel(false)
    {}

    bool LoginListener::init(String controllerName)
    {
        super::init(controllerName);
        m_owner = (SceneTitle*)m_pCurrentScene;

        m_keyListener = cocos2d::EventListenerKeyboard::create();
        m_keyListener->onKeyPressed = CC_CALLBACK_2(LoginListener::onKeyPressed, this);
        m_keyListener->onKeyReleased = CC_CALLBACK_2(LoginListener::onKeyReleased, this);
        cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_keyListener, 1);

        getUIController()->playUIAnimation("ShakeLight");

        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_StartGame, this), "BTN_Start");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_Chapt1, this), "BTN_Chapt1");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_NextChapt, this), "BTN_NextChapt");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_PrevChapt, this), "BTN_PrevChapt");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_Level_0, this), "IMG_Level_0");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_Level_1, this), "IMG_Level_1");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_Level_2, this), "IMG_Level_2");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_Credit_Show, this), "BTN_Credit");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::IMG_Credit_Close, this), "IMG_Credit");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_ExitGame, this), "BTN_ExitGame");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(LoginListener::BTN_BackToGame, this), "BTN_BackToGame");

        // audio
        AudioManager::getInstance()->preLoad(1);
        AudioManager::getInstance()->playBGM("Title");

        return true;
    }

    bool LoginListener::uninit()
    {
        cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(m_keyListener);

        return super::uninit();
    }

    void LoginListener::update(f32 dt)
    {
        if (m_curCd > 0.0f)
        {
            m_curCd -= dt;
            m_curCd = m_curCd > 0.0f ? m_curCd : 0.0f;
        }
        if (m_curSession != eMS_Title) return;
        m_lightTime += dt;
        if (m_lightTime > m_lightCd)
        {
            m_lightCd = cocos2d::random(0.5f, 10.0f);
            ECHO_TRACE("DEBUG", "%.2f", m_lightCd);
            m_lightTime = 0.0f;
            auto lightImage = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName("IMG_Light2"));
            lightImage->loadTexture("ui/IMG_TitleLight4.png");
            lightImage->runAction(cocos2d::Sequence::create(
                cocos2d::DelayTime::create(0.1f),
                cocos2d::CallFunc::create([=](){
                lightImage->loadTexture("ui/IMG_TitleLight2.png");
                AudioManager::getInstance()->playSE("light_flash");
            }),
                NULL
                ));
        }
    }

    bool LoginListener::BTN_StartGame(String /*uid*/)
    {
        if (m_started) return true;

        if (m_curCd > MATH_EPSILON) return true;

        auto lightImage = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName("IMG_Light2"));
        switch (m_curSession)
        {
        case Echo::eMS_Title:
            AudioManager::getInstance()->playSE("switch");
            getUIController()->playUIAnimation("TitleToChapter");
            lightImage->loadTexture("ui/IMG_TitleLight4.png");
            m_curCd = COOLDOWN;
            m_curSession = eMS_Chapter;
            break;
        case Echo::eMS_Chapter:
            AudioManager::getInstance()->playSE("switch");
            getUIController()->playUIAnimation("ChapterToTitle");
            lightImage->loadTexture("ui/IMG_TitleLight2.png");
            m_curCd = COOLDOWN;
            m_curSession = eMS_Title;
            break;
        case Echo::eMS_Level:
            AudioManager::getInstance()->playSE("switch");
            getUIController()->playUIAnimation("LevelToTitle");
            lightImage->loadTexture("ui/IMG_TitleLight2.png");
            m_curCd = COOLDOWN;
            m_curSession = eMS_Title;
            break;
        default:
            ECHO_ASSERT(false, "");
            break;
        }
        return true;
    }

    bool LoginListener::BTN_Chapt1(String uid)
    {
        AudioManager::getInstance()->playSE("changeimage");
        m_curSession = eMS_Level;
        getUIController()->playUIAnimation("ChapterToLevel");

        return true;
    }

    bool LoginListener::BTN_NextChapt(String uid)
    {
        if (m_started) return true;

        ECHO_ASSERT(m_curSession == eMS_Level, "");

        u32 prevLevel = m_curLevel;
        m_curLevel += 1;
        if (m_curLevel > MAX_LEVEL)
        {
            m_curLevel = 0;
        }
        changeLevel(prevLevel, m_curLevel);
        return true;
    }

    bool LoginListener::BTN_PrevChapt(String uid)
    {
        if (m_started) return true;

        ECHO_ASSERT(m_curSession == eMS_Level, "");

        u32 prevLevel = m_curLevel;
        m_curLevel -= 1;
        if (s32(m_curLevel) < 0)
        {
            m_curLevel = MAX_LEVEL;
        }
        changeLevel(prevLevel, m_curLevel);
        return true;
    }

    void LoginListener::changeLevel(u32 prevLevel, u32 nextLevel)
    {
        getUIController()->playUIAnimation("ChangeLevel");
        AudioManager::getInstance()->playSE("changeimage");
        
        String prevWidgetName = ECHO_STR("IMG_Level_%d", prevLevel);
        auto prevLevelWidget = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName(prevWidgetName));
        ECHO_ASSERT(prevLevelWidget, "Cannot get levelWidget %s", prevWidgetName.c_str());

        String nextWidgetName = ECHO_STR("IMG_Level_%d", nextLevel);
        auto nextLevelWidget = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName(nextWidgetName));
        ECHO_ASSERT(nextLevelWidget, "Cannot get levelWidget %s", nextWidgetName.c_str());

        auto fadeOutAction = cocos2d::Sequence::create(
            cocos2d::FadeOut::create(0.1f),
            cocos2d::MoveTo::create(0.01f, cocos2d::Vec2(-500, 2000)),
            NULL
            );
        auto fadeInAction = cocos2d::Sequence::create(
            cocos2d::DelayTime::create(0.4f),
            cocos2d::MoveTo::create(0.01f, cocos2d::Vec2(360, 800)),
            cocos2d::FadeIn::create(0.1f),
            NULL
            );
        prevLevelWidget->runAction(fadeOutAction);
        nextLevelWidget->runAction(fadeInAction);
    }

    bool LoginListener::BTN_Level_0(String uid)
    {
        if (m_started) return true;

        GameLogic::getInstance()->setCurLevelName("Level_0");
        startGame(0);
        return true;
    }

    bool LoginListener::BTN_Level_1(String uid)
    {
        if (m_started) return true;

        GameLogic::getInstance()->setCurLevelName("Level_1");
        startGame(1);
        return true;
    }

    bool LoginListener::BTN_Level_2(String uid)
    {
        if (m_started) return true;

        GameLogic::getInstance()->setCurLevelName("Level_2");
        startGame(2);
        return true;
    }

    void LoginListener::startGame(u32 level)
    {
        m_started = true;
        getUIController()->playUIAnimation("LevelToGame");
        auto levelWidget = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName(ECHO_STR("IMG_Level_%d", level)));
        auto finalAction = cocos2d::Sequence::create(
            cocos2d::DelayTime::create(0.3f),
            cocos2d::ScaleTo::create(0.5f, 1.5f),
            cocos2d::DelayTime::create(0.3f),
            cocos2d::CallFunc::create([=](){
            GameLogic::getInstance()->switchScene("SceneLoadMap");
        }),
            NULL
            );
        levelWidget->runAction(finalAction);
    }

    bool LoginListener::BTN_Credit_Show(String uid)
    {
        if (!m_showCredit)
        {
            m_showCredit = true;
            getUIController()->playUIAnimation("Info_Show");
        }
        return true;
    }

    bool LoginListener::IMG_Credit_Close(String uid)
    {
        if (m_showCredit)
        {
            m_showCredit = false;
            getUIController()->playUIAnimation("Info_Close");
        }
        return true;
    }

    void LoginListener::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* /*event*/)
    {
        if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_BACK)
        {
            if (m_showExitPanel)
            {
                this->BTN_BackToGame("");
            }
            else
            {
                getUIController()->playUIAnimation("ExitGame");
                m_showExitPanel = true;
            }
        }
    }

    void LoginListener::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* /*event*/)
    {
        ECHO_UNUSED(keyCode);
    }

    bool LoginListener::BTN_ExitGame(String uid)
    {
        ECHO_END_GAME;

        return true;
    }

    bool LoginListener::BTN_BackToGame(String uid)
    {
        if (m_showExitPanel)
        {
            m_showExitPanel = false;
            getUIController()->playUIAnimation("BackToGame");
        }
        return true;
    }

}