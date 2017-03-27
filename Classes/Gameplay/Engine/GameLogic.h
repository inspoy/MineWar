//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_GAMELOGIC_H__INCLUDED
#define ECHO_GAMEPLAY_GAMELOGIC_H__INCLUDED

#include "Precompiled.h"
#include "cocos2d.h"

namespace Echo
{

    class GameScene;
    class BaseGameController;

    typedef std::map<StdString, std::vector<BaseGameController*> > ControllerMap;

    class GameLogic
    {
    public:
        ECHO_DECLARE_SINGLETON(GameLogic);
    public:
        bool init();
        void update(float dt);

        GameScene* switchScene(String sceneName, bool dontSwitch = false, bool noEffect =false);
        void setCurrentScene(GameScene* pScene);
        template<class _T>
        _T getCurrentScene()
        {
            _T ret = dynamic_cast<_T>(m_currentScene);
            return ret;
        }
        
        void registerGameController(BaseGameController* pController);
        void unregisterGameController(BaseGameController* pController);

        // other
        String getCurLevelName();
        void setCurLevelName(String levelName);

        // tools
        u32 getNewUid();

    private:
        cocos2d::Director* getDirector();
        void onKeyPressed(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event* event);

    private:
        cocos2d::Director* m_director;

        GameScene* m_currentScene;
        String m_currentSceneName;
        ControllerMap m_gameControllerMap;
        String m_curLevelName;
        u32 m_uidCount;
        u32 m_shotCount;
    };

}

#endif