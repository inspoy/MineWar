//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_SCENES_LOGINLISTENER_H__INCLUDED
#define ECHO_SCENES_LOGINLISTENER_H__INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/BaseGameController.h"

namespace Echo
{

    enum EMenuSession
    {
        eMS_Title = 0,
        eMS_Chapter = 1,
        eMS_Level = 2,

        eMS_Count
    };

    class SceneTitle;

    class LoginListener : public BaseGameController
    {
    public:
        LoginListener();
        bool init(String controllerName);
        bool uninit();
        void update(f32 dt);

        //===================================

        static const u32 MAX_LEVEL;

    private:
        void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

        // For UI
        bool BTN_StartGame(String uid);
        bool BTN_Chapt1(String uid);
        bool BTN_NextChapt(String uid);
        bool BTN_PrevChapt(String uid);
        bool BTN_Level_0(String uid);
        bool BTN_Level_1(String uid);
        bool BTN_Level_2(String uid);
        bool BTN_Credit_Show(String uid);
        bool IMG_Credit_Close(String uid);
        bool BTN_ExitGame(String uid);
        bool BTN_BackToGame(String uid);
        // UI End
        void changeLevel(u32 prevLevel, u32 nextLevel);
        void startGame(u32 level);

    private:
        SceneTitle* m_owner;
        cocos2d::EventListenerKeyboard* m_keyListener;
        EMenuSession m_curSession;
        f32 m_curCd;
        f32 m_lightTime;

        f32 m_lightCd;
        u32 m_curLevel;

        static const f32 COOLDOWN;
        bool m_started;
        bool m_showCredit;
        bool m_showExitPanel;
    };

}

#endif