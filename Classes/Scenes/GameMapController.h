//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_SCENES_GAMEMAPCONTROLLER_H__INCLUDED
#define ECHO_SCENES_GAMEMAPCONTROLLER_H__INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "Gameplay/Engine/BaseGameController.h"
#include "Gameplay/Engine/MetaDataHelper.h"
#include "Gameplay/MapDefinitions.h"

namespace Echo
{

    class SceneGameMap;
    class ArrowManager;
    class BaseBossController;

    class GameMapController : public BaseGameController
    {
    public:
        GameMapController();
        bool init(String controllerName);
        bool uninit();
        void update(f32 dt);

        // ==============GAME=================
        void setPause(bool val);
        bool getPause();
        void setInMiniGame(bool val);
        bool getInMiniGame();

        // ===============HERO==================
        cocos2d::Sprite* getHeroSprite();
        void addHp(f32 val);
        bool reduceHp(f32 val);

        // ==============MOVE=================
        void moveHero(EMoveDirection direction);
        void refreshSight(u32 x, u32 y, bool getOrLost);
        void digTile(u32 x, u32 y, u32 quadrant, bool undig = false);
        void clearPos(u32 x, u32 y, bool undig = false);
        u32 getDigGID(u32 oldGID, u32 quadrantMask, u8& newMask, bool undig);
        
        // ===========TREASURE===========
        void addCoin(u32 amount);

        // =============BOSS=============
        void addBoss(BaseBossController* controller);
        void removeBoss(BaseBossController* controller);
        u32 getBossCount();
        void clearBosses();

        // =============UTIL=============
        u32 getHeroCurPosX() const;
        u32 getHeroCurPosY() const;
        MapPoint getHeroCurPos() const;
        SceneGameMap* getOwner() const;
        ArrowManager* getArrowManager() const;
        static cocos2d::Vec2 getRealPos(u32 x, u32 y);
        static cocos2d::Vec2 getRealPos(MapPoint pos);
        static u32 getMapHeight();
        static u32 getMapWidth();
        void checkEntityReached(f32 dt);
        bool checkHeroReach(u32 x, u32 y);
        bool posValid(MapPoint pos);

        // Player
        void gameOver(bool win);
        void addRealScore(u32 val);
        void addTreasureCount(u32 coin);

    private:
        // callback
        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        void onSingleClick(cocos2d::Touch* touch, cocos2d::Event* event);
        void onDoubleClick(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
        void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
        void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
        void onMoveEnded();

        // UI
        bool BTN_CloseInitBox(String uid);
        bool BTN_Pause(String uid);
        bool BTN_Resume(String uid);
        bool BTN_Retry(String uid);
        bool BTN_Return(String uid);
        bool BTN_Clear_Retry(String uid);
        bool BTN_Clear_Next(String uid);
        bool BTN_Clear_Return(String uid);

        // Hp and Score
        bool updateHp(f32 dt);
        void updateHpBar(u32 i);
        void updateScore(f32 dt);

        // Boss
        void updateBosses(f32 dt);


    private:
        SceneGameMap* m_owner;
        ArrowManager* m_arrowManager;
        cocos2d::EventListenerTouchOneByOne* m_touchListener;
        cocos2d::EventListenerKeyboard* m_keyListener;
        cocos2d::Sprite* m_heroSprite;

        bool m_gameOver;
        bool m_paused;
        bool m_pausePanelShowed;
        bool m_inMiniGame;
        bool m_moving;
        bool m_touching;
        f32 m_prevPosX;
        f32 m_prevPosY;

        f32 m_curBombCd;

        f32 m_curHp;
        u32 m_curHpBar;
        f32 m_curScoreCd;
        u32 m_realScore;
        u32 m_dispScore;
        f32 m_timeScore;

        f32 m_moveTime;
        f32 m_speed;
        EMoveDirection m_curDirection;
        u32 m_range;
        u32 m_heroCurPosX;
        u32 m_heroCurPosY;
        std::vector<CubeInfo> m_mainMap;
        bool m_willChangeMap;

        std::vector<BaseBossController*> m_bosses;

        // statistical
        f32 m_curTime;
        u32 m_supplyCount;
        u32 m_treasureCount;
        u32 m_coinCount;

        // touch
        bool m_isDoubleClick;
        f32 m_preClickTime;
        f32 m_curClickTime;
        f32 m_backCd;
        
    public:
        static const u32 MAP_WIDTH;
        static const u32 MAP_HEIGHT;
        static const u32 MAX_HP;
        static const u32 MIN_HP;
        static const u32 MOVE_THEREHOLD;
        static const u32 DPS_COMMON;
        static const u32 SIZE_PER_BLOCK;
        static const u32 HERO_FRAME_COUNT;
        static const f32 MYBOMB_COOLDOWN;
    };

}

#endif
