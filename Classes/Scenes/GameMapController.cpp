//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Scenes/GameMapController.h"
#include "Scenes/SceneGameMap.h"
#include "ui/UIText.h"
#include "2d/CCTMXTiledMap.h"
#include "ui/UILoadingBar.h"
#include "Gameplay/MapDefinitions.h"
#include "Gameplay/ArrowManager.h"
#include "Gameplay/BossController.h"
#include "Gameplay/KeyAndDoor.h"
#include "Gameplay/Boss.h"
#include "Gameplay/Engine/GameLogic.h"
#include "ui/UIImageView.h"
#include "Gameplay/Treasure.h"
#include "Gameplay/Bomb.h"
#include "ui/UIButton.h"
#include "ui/UITextAtlas.h"
#include "cocostudio/CCComRender.h"
#include "Utilities/TimeUtl.h"
#include "Gameplay/MyBomb.h"
#include "CCFileUtils.h"
#include "Gameplay/Engine/AudioManager.h"
#include "LoginListener.h"

namespace Echo
{

    const u32 GameMapController::MAP_WIDTH = 9;
    const u32 GameMapController::MAP_HEIGHT = 15;
    const u32 GameMapController::MAX_HP = 100;
    const u32 GameMapController::MIN_HP = 0;
    const u32 GameMapController::MOVE_THEREHOLD = 100;
    const u32 GameMapController::DPS_COMMON = 2; // reduce 2 of max HP per second
    const u32 GameMapController::SIZE_PER_BLOCK = 80;
    const u32 GameMapController::HERO_FRAME_COUNT = 6;
    const f32 GameMapController::MYBOMB_COOLDOWN = 5.0f;


    GameMapController::GameMapController()
        : m_owner(NULL)
        , m_heroSprite(NULL)
        , m_gameOver(false)
        , m_paused(true)
        , m_pausePanelShowed(false)
        , m_inMiniGame(false)
        , m_moving(false)
        , m_touching(false)
        , m_curScoreCd(0.0f)
        , m_realScore(0)
        , m_dispScore(0)
        , m_timeScore(0.0f)
        , m_moveTime(0.0f)
        , m_speed(3.6f)
        , m_curDirection(eMD_Invalid)
        , m_range(1)
        , m_willChangeMap(false)
        , m_curTime(0.0f)
        , m_supplyCount(0)
        , m_treasureCount(0)
        , m_coinCount(0)
        , m_isDoubleClick(false)
        , m_preClickTime(0.0f)
        , m_curClickTime(0.0f)
        , m_backCd(0.0f)
    {
    }

    bool GameMapController::init(String controllerName)
    {
        super::init(controllerName);
        m_owner = (SceneGameMap*)m_pCurrentScene;

        m_touchListener = cocos2d::EventListenerTouchOneByOne::create();
        m_touchListener->onTouchBegan = CC_CALLBACK_2(GameMapController::onTouchBegan, this);
        m_touchListener->onTouchMoved = CC_CALLBACK_2(GameMapController::onTouchMoved, this);
        m_touchListener->onTouchEnded = CC_CALLBACK_2(GameMapController::onTouchEnded, this);
        cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_touchListener, m_owner->getMapNode());
        //cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_touchListener, 1);

        m_keyListener = cocos2d::EventListenerKeyboard::create();
        m_keyListener->onKeyPressed = CC_CALLBACK_2(GameMapController::onKeyPressed, this);
        m_keyListener->onKeyReleased = CC_CALLBACK_2(GameMapController::onKeyReleased, this);
        cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_keyListener, 1);

        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_CloseInitBox, this), "IMG_HintBox");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Pause, this), "BTN_Pause");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Resume, this), "BTN_Resume");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Retry, this), "BTN_Retry");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Return, this), "BTN_Return");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Clear_Retry, this), "BTN_Clear_Retry");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Clear_Next, this), "BTN_Clear_Next");
        getUIController()->registerAPIFunction(CC_CALLBACK_1(GameMapController::BTN_Clear_Return, this), "BTN_Clear_Return");

        // other
        m_heroCurPosX = m_owner->getCurMapInfo().m_spawnPos.m_x;
        m_heroCurPosY = m_owner->getCurMapInfo().m_spawnPos.m_y;
        m_owner->getHeroNode()->setPosition(getRealPos(m_heroCurPosX, m_heroCurPosY));

        MapInfo& firstMap = m_owner->getCurMapInfo();
        String directionString = firstMap.m_spawnPos.m_goalDirection;
        m_curDirection = eMD_Invalid;
        if (directionString == "top")
        {
            m_curDirection = eMD_Top;
        }
        else if (directionString == "bottom")
        {
            m_curDirection = eMD_Bottom;
        }
        else if (directionString == "left")
        {
            m_curDirection = eMD_Left;
        }
        else if (directionString == "right")
        {
            m_curDirection = eMD_Right;
        }
        m_heroCurPosX = firstMap.m_spawnPos.m_x;
        m_heroCurPosY = firstMap.m_spawnPos.m_y;

        m_curHp = MAX_HP;
        m_curHpBar = 10;

        m_mainMap.resize((MAP_WIDTH + 1) * MAP_HEIGHT + 1);
        for (u32 i = 1; i <= MAP_WIDTH; ++i)
        {
            for (u32 j = 1; j <= MAP_HEIGHT; ++j)
            {
                CubeInfo& ci = m_mainMap[i * MAP_HEIGHT + j];
                ci.m_x = i;
                ci.m_y = j;
                ci.m_statusCube = 16;
            }
        }

        m_arrowManager = new ArrowManager();
        m_arrowManager->setController(this);
        m_arrowManager->initTraps(m_owner->getCurEntities());

        auto hintBox = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName("IMG_Hint"));
        hintBox->loadTexture(ECHO_STR("maps/%sHint.png", GameLogic::getInstance()->getCurLevelName().c_str()));
        getUIController()->playUIAnimation("InitBox_Show");

        // hero animation
        auto heroNode = m_owner->getHeroNode();
        auto heroCom = dynamic_cast<cocostudio::ComRender*>(heroNode->getComponent("CCSprite"));
        auto heroSprite = dynamic_cast<cocos2d::Sprite*>(heroCom->getNode());
        auto heroAnimation = cocos2d::Animation::create();
        for (u32 i = 1; ; ++i)
        {
            auto idStr = String(i, 4, '0');
            String fp = ECHO_STR("animation/hero/Hero%s.png", idStr.c_str());
            if (cocos2d::FileUtils::getInstance()->isFileExist(fp.c_str()))
            {
                heroAnimation->addSpriteFrameWithFile(fp.c_str());
            }
            else
            {
                break;
            }
        }
        heroAnimation->setDelayPerUnit(0.04f); // delay between 2 frames, 25fps
        heroAnimation->setLoops(1);
        auto heroAnimate = cocos2d::Animate::create(heroAnimation);
        auto heroAnimateWithSound = cocos2d::Spawn::createWithTwoActions(
            heroAnimate,
            cocos2d::CallFunc::create([=](){
            AudioManager::getInstance()->playSE("hero_move");
        }));
        heroSprite->runAction(cocos2d::RepeatForever::create(heroAnimateWithSound));
        heroSprite->setAnchorPoint(cocos2d::Vec2(0.5f, 0.38f));
        
        // Audio
        AudioManager::getInstance()->preLoad(2);
        AudioManager::getInstance()->playBGM("", true);
        AudioManager::getInstance()->getEngine()->setEffectsVolume(0.0f);

        // Cheat
#ifdef ECHO_CHEAT
        m_speed = 5.0f;
#endif

        return true;
    }

    bool GameMapController::uninit()
    {
        CC_SAFE_DELETE(m_arrowManager);

        return super::uninit();
    }

    void GameMapController::update(f32 dt)
    {
        if (m_backCd > 0.0f) m_backCd -= dt;
        if (m_paused && !m_gameOver) return;

        if (m_gameOver)
        {
            updateScore(dt);
            return;
        }

        for (auto ent : m_owner->getCurEntities())
        {
            if (!m_inMiniGame || ent->getType() == eET_Bomb || ent->getType() == eET_TreasureBox)
            {
                ent->update(dt);
            }
        }
        
        if (!m_inMiniGame)
        {
            m_timeScore += dt;
            if (m_timeScore > 1.0f)
            {
                m_timeScore -= 1.0f;
                ++m_realScore;
            }
            m_curTime += dt;
            m_curBombCd -= dt;
            if (m_curBombCd < 0.0f)
            {
                m_curBombCd = 0.0f;
            }

            if (!m_moving && m_touching)
            {
                moveHero(m_curDirection);
            }

            m_arrowManager->update(dt);

            updateBosses(dt);
            
            checkEntityReached(dt);

            updateScore(dt);

            if (!updateHp(dt))
            {
                gameOver(false);
            }
        }
    }

    bool GameMapController::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
    {
        // ECHO_TRACE_NOARG("UI", "onTouchBegan called.");
        m_preClickTime = m_curClickTime;
        m_curClickTime = TimeUtl::getInstance()->getRunTime().toSec();
        if (m_curClickTime - m_preClickTime > 0.3f || m_isDoubleClick)
        {
            onSingleClick(touch, event);
            m_isDoubleClick = false;
        }
        else
        {
            onDoubleClick(touch, event);
            m_isDoubleClick = true;
        }

        return true;
    }

    void GameMapController::onSingleClick(cocos2d::Touch* touch, cocos2d::Event* /*event*/)
    {
        m_prevPosX = touch->getStartLocation().x;
        m_prevPosY = touch->getStartLocation().y;
    }

    void GameMapController::onDoubleClick(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {
        // release my bomb
        ECHO_TRACE_NOARG("UI", "double click!");
        if ((m_curBombCd < MATH_EPSILON) && !m_moving && (m_owner->getEntity(m_heroCurPosX, m_heroCurPosY) == NULL))
        {
            SceneGameMap* owner = GameLogic::getInstance()->getCurrentScene<SceneGameMap*>();
            auto newBomb = Entity::create("MyBomb_Basic", eET_MyBomb);
            newBomb->setType(eET_MyBomb);
            newBomb->setGameMapController(this);
            newBomb->setPosX(m_heroCurPosX);
            newBomb->setPosY(m_heroCurPosY);
            owner->addEntity(newBomb);
            m_curBombCd = MYBOMB_COOLDOWN;
        }
    }

    void GameMapController::onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* /*event*/)
    {
        if (m_paused) return;
        if (m_inMiniGame) return;
        // if (m_touching) return;
        // if (m_isDoubleClick) return;
        f32 dx = touch->getLocation().x - m_prevPosX;
        f32 dy = touch->getLocation().y - m_prevPosY;

        f32 dis = dx * dx + dy * dy;

        if (dis > MOVE_THEREHOLD)
        {
            m_prevPosX = touch->getLocation().x;
            m_prevPosY = touch->getLocation().y;
            m_touching = true;
            EMoveDirection dir = eMD_Invalid;
            if (abs(dx) > abs(dy))
            {
                if (dx > 0)
                {
                    dir = eMD_Right;
                    // getUIController()->playUIAnimation("Move_Right_Begin");
                }
                else
                {
                    // getUIController()->playUIAnimation("Move_Left_Begin");
                    dir = eMD_Left;
                }
            }
            else
            {
                if (dy > 0)
                {
                    // getUIController()->playUIAnimation("Move_Up_Begin");
                    dir = eMD_Top;
                }
                else
                {
                    // getUIController()->playUIAnimation("Move_Down_Begin");
                    dir = eMD_Bottom;
                }
            }
            auto heroSprite = getHeroSprite();
            switch (dir)
            {
            case Echo::eMD_Top:
                break;
            case Echo::eMD_Left:
                heroSprite->setScaleX(-1.0f);
                break;
            case Echo::eMD_Bottom:
                break;
            case Echo::eMD_Right:
                heroSprite->setScaleX(1.0f);
                break;
            default:
                ECHO_ASSERT(false, "");
                break;
            }
            m_curDirection = dir;
        }
    }

    void GameMapController::onTouchEnded(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {
        if (m_touching)
        {
            m_touching = false;
//             switch (m_curDirection)
//             {
//             case Echo::eMD_Top:
//                 getUIController()->playUIAnimation("Move_Up_End");
//                 break;
//             case Echo::eMD_Left:
//                 getUIController()->playUIAnimation("Move_Left_End");
//                 break;
//             case Echo::eMD_Bottom:
//                 getUIController()->playUIAnimation("Move_Down_End");
//                 break;
//             case Echo::eMD_Right:
//                 getUIController()->playUIAnimation("Move_Right_End");
//                 break;
//             default:
//                 ECHO_ASSERT(false, "Invalid direction.");
//                 break;
//             }
        }
    }

    void GameMapController::moveHero(EMoveDirection direction)
    {
        u32 goalPosX = m_owner->getCurMapInfo().m_goalPos.m_x;
        u32 goalPosY = m_owner->getCurMapInfo().m_goalPos.m_y;
        bool reachGoal = (goalPosX == m_heroCurPosX) && (goalPosY == m_heroCurPosY);
        String goalDir = m_owner->getCurMapInfo().m_goalPos.m_goalDirection;

        f32 curPosX = m_owner->getHeroNode()->getPositionX();
        f32 curPosY = m_owner->getHeroNode()->getPositionY();
        bool moved = false;

        // lost sight
        refreshSight(0, 0, false);

        switch (direction)
        {
        case Echo::eMD_Top:
            if ((m_heroCurPosY < MAP_HEIGHT - 1) || (reachGoal && goalDir == "top"))
            {
                bool reachFlag = false;
                std::vector<Entity*> ent = m_owner->getCurEntities();
                for (auto curEnt : ent)
                {
                    if (curEnt->getBlock())
                    {
                        if (curEnt->getPosX() == m_heroCurPosX && curEnt->getPosY()-1 == m_heroCurPosY)
                        {
                            reachFlag = true;
                            break;
                        }
                    }
                }
                if (!reachFlag)
                {
                    m_heroCurPosY += 1;
                    curPosY += SIZE_PER_BLOCK;
                    moved = true;
                    m_willChangeMap = reachGoal && goalDir == "top";
                }
            }
            break;
        case Echo::eMD_Left:
            if ((m_heroCurPosX > 1) || (reachGoal && goalDir == "left"))
            {
                bool reachFlag = false;
                std::vector<Entity*> ent = m_owner->getCurEntities();
                for (auto curEnt : ent)
                {
                    if (curEnt->getBlock())
                    {
                        if (curEnt->getPosX()+1 == m_heroCurPosX && curEnt->getPosY() == m_heroCurPosY)
                        {
                            reachFlag = true;
                            break;
                        }
                    }
                }
                if (!reachFlag)
                {
                    m_heroCurPosX -= 1;
                    curPosX -= SIZE_PER_BLOCK;
                    moved = true;
                    m_willChangeMap = reachGoal && goalDir == "left";
                }
            }
            break;
        case Echo::eMD_Bottom:
            if ((m_heroCurPosY > 1) || (reachGoal && goalDir == "bottom"))
            {
                bool reachFlag = false;
                std::vector<Entity*> ent = m_owner->getCurEntities();
                for (auto curEnt : ent)
                {
                    if (curEnt->getBlock())
                    {
                        if (curEnt->getPosX() == m_heroCurPosX && curEnt->getPosY()+1 == m_heroCurPosY)
                        {
                            reachFlag = true;
                            break;
                        }
                    }
                }
                if (!reachFlag)
                {
                    m_heroCurPosY -= 1;
                    curPosY -= SIZE_PER_BLOCK;
                    moved = true;
                    m_willChangeMap = reachGoal && goalDir == "bottom";
                }
            }
            break;
        case Echo::eMD_Right:
            if ((m_heroCurPosX < MAP_WIDTH - 1) || (reachGoal && goalDir == "right"))
            {
                bool reachFlag = false;
                std::vector<Entity*> ent = m_owner->getCurEntities();
                for (auto curEnt : ent)
                {
                    if (curEnt->getBlock())
                    {
                        if (curEnt->getPosX()-1 == m_heroCurPosX && curEnt->getPosY() == m_heroCurPosY)
                        {
                            reachFlag = true;
                            break;
                        }
                    }
                }
                if (!reachFlag)
                {
                    m_heroCurPosX += 1;
                    curPosX += SIZE_PER_BLOCK;
                    moved = true;
                    m_willChangeMap = reachGoal && goalDir == "right";
                }
            }
            break;
        default:
            ECHO_ASSERT(false, "Unknown direction!");
            break;
        }
        if (moved)
        {
            // get new sight
            refreshSight(0, 0, true);
            m_moving = true;
            auto moveAction = cocos2d::MoveTo::create(1 / m_speed, cocos2d::Vec2(curPosX, curPosY));
            m_owner->getHeroNode()->runAction(cocos2d::Sequence::createWithTwoActions(moveAction, cocos2d::CallFunc::create(CC_CALLBACK_0(GameMapController::onMoveEnded, this))));
            if (reachGoal && goalDir == "center")
            {
                m_willChangeMap = true;
            }
        }
    }

    void GameMapController::refreshSight(u32 x, u32 y, bool getOrLost)
    {
        // ECHO_TRACE("GameMap","%s Sight(%d,%d)", getOrLost ? "Get" : "Lost", x, y);
        if (x == 0 && y == 0)
        {
            // default
            x = m_heroCurPosX;
            y = m_heroCurPosY;
        }

        for (u32 xx = x - m_range; xx <= x + m_range; ++xx)
        {
            if (xx < 1 || xx > MAP_WIDTH - 1) continue;

            for (u32 yy = y - m_range; yy <= y + m_range; ++yy)
            {
                if (yy < 1 || yy > MAP_HEIGHT - 1) continue;

                // get or lost xx,yy
                auto curEnt = m_owner->getEntity(xx, yy);
                if (curEnt == NULL) continue;

                if (!curEnt->getFound())
                {
#ifdef COCOS2D_DEBUG
                    if (curEnt->getType() != eET_Terrain)
                    {
                        ECHO_TRACE("GameMap", "Item %d Found!", curEnt->getUid());
                    }
#endif
                    curEnt->setFound(true);
                }
                if (getOrLost && curEnt->getHide())
                {
                    curEnt->setHide(false);
                }
                if (!getOrLost && !curEnt->getHide())
                {
                    curEnt->setHide(true);
                }
            }
        }
    }

    void GameMapController::digTile(u32 x, u32 y, u32 quadrant, bool undig)
    {
        if (m_willChangeMap) return;

        if (x < 1 || x > MAP_WIDTH || y < 1 || y > MAP_HEIGHT)
        {
            ECHO_TRACE("GameMap", "pos(%d, %d) OoR, will return...", x, y);
            return;
        }

        CubeInfo& cinfo = m_mainMap[x * MAP_HEIGHT + y];
        // verify
        ECHO_ASSERT(cinfo.m_x == x && cinfo.m_y == y, "CubeInfo position mismatch.");

        MapPoint tilePos(x - 1, MAP_HEIGHT - y);
        auto tmxMap = dynamic_cast<cocos2d::TMXTiledMap*>(m_owner->getMapNode());
        auto dirtLayer = tmxMap->getLayer("wall");
        u32 dirtTileGID = dirtLayer->getTileGIDAt(tilePos.toVec2());
        u32 qMask = quadrant == 3 ? 4 : (quadrant == 4 ? 8 : quadrant);
        u32 newGID = getDigGID(dirtTileGID, qMask, cinfo.m_statusCube, undig);
        dirtLayer->setTileGID(newGID, tilePos.toVec2());
    }

    void GameMapController::clearPos(u32 x, u32 y, bool undig)
    {
        auto ent = m_owner->getEntity(x, y);
        if (!ent) return;
        if (ent->getType() != eET_Terrain) return;
//         digTile(x, y, 1, undig);
//         digTile(x + 1, y, 2, undig);
//         digTile(x + 1, y + 1, 3, undig);
//         digTile(x, y + 1, 4, undig);

        if (!undig)
        {
            m_owner->playAnimation("DestroyWall", x, y);
        }
    }

    void GameMapController::onMoveEnded()
    {
        m_moving = false;
        // ECHO_TRACE("GameMap", "Move Ended, curPos is %f, %f", m_owner->getHeroNode()->getPositionX(), m_owner->getHeroNode()->getPositionY());

        if (m_willChangeMap)
        {
            if (m_owner->getCurMapInfo().m_id < 3)
            {
                // change map
                m_willChangeMap = false;
                ECHO_TRACE("GameMap", "Will change map to %d...", m_owner->getCurMapInfo().m_id + 1);

                // remove old touch listener
                cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListener(m_touchListener);

                // update map node
                m_owner->changeMap(m_owner->getCurMapInfo().m_id + 1);

                // create new touch listener
                m_touchListener = cocos2d::EventListenerTouchOneByOne::create();
                m_touchListener->onTouchBegan = CC_CALLBACK_2(GameMapController::onTouchBegan, this);
                m_touchListener->onTouchMoved = CC_CALLBACK_2(GameMapController::onTouchMoved, this);
                m_touchListener->onTouchEnded = CC_CALLBACK_2(GameMapController::onTouchEnded, this);
                cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_touchListener, m_owner->getMapNode());

                // init hero pos
                m_heroCurPosX = m_owner->getCurMapInfo().m_spawnPos.m_x;
                m_heroCurPosY = m_owner->getCurMapInfo().m_spawnPos.m_y;
                m_owner->getHeroNode()->setPosition(getRealPos(m_heroCurPosX, m_heroCurPosY));
                refreshSight(m_heroCurPosX, m_heroCurPosY, true);

                // init traps
                m_arrowManager->initTraps(m_owner->getCurEntities());

                // reset control
                onTouchEnded(NULL, NULL);

                // change BGM
                if (m_owner->getCurMapInfo().m_id == 3)
                {
                    AudioManager::getInstance()->playBGM("Boss");
                }
            }
            else
            {
                ECHO_END_GAME;
            }
        }
    }

    bool GameMapController::BTN_CloseInitBox(String /*uid*/)
    {
        if (m_paused)
        {
            getUIController()->playUIAnimation("InitBox_Close");
            m_paused = false;

            AudioManager::getInstance()->getEngine()->setEffectsVolume(1.0f);
            AudioManager::getInstance()->playBGM("MainTheme");
        }

        return true;
    }

    bool GameMapController::BTN_Pause(String /*uid*/)
    {
        if (!m_paused)
        {
            m_paused = true;
            m_pausePanelShowed = true;
            getUIController()->playUIAnimation("PausePanel_Show");
            AudioManager::getInstance()->playSE("changeimage");
        }

        return true;
    }

    bool GameMapController::BTN_Resume(String /*uid*/)
    {
        getUIController()->playUIAnimation("PausePanel_Close");
        m_paused = false;
        m_pausePanelShowed = false;

        return true;
    }

    bool GameMapController::BTN_Retry(String /*uid*/)
    {
        GameLogic::getInstance()->switchScene("SceneGameMap");

        return true;
    }

    bool GameMapController::BTN_Return(String /*uid*/)
    {
        GameLogic::getInstance()->switchScene("SceneTitle");

        return true;
    }

    bool GameMapController::BTN_Clear_Retry(String /*uid*/)
    {
        GameLogic::getInstance()->switchScene("SceneGameMap");

        return true;
    }

    bool GameMapController::BTN_Clear_Next(String /*uid*/)
    {
        auto gameLogic = GameLogic::getInstance();
        String curLevelName = gameLogic->getCurLevelName();
        if (curLevelName == ECHO_STR("Level_%d", LoginListener::MAX_LEVEL))
        {
            gameLogic->switchScene("SceneTitle");
        }
        else
        {
            String nextLevel;
            u32 curLevelNo = String(curLevelName.stdStr.substr(6)).toU32();
            nextLevel = ECHO_STR("Level_%d", curLevelNo + 1);
            gameLogic->setCurLevelName(nextLevel);
            gameLogic->switchScene("SceneGameMap");
        }

        return true;
    }

    bool GameMapController::BTN_Clear_Return(String /*uid*/)
    {
        GameLogic::getInstance()->switchScene("SceneTitle");

        return true;
    }

    u32 GameMapController::getDigGID(u32 oldGID, u32 quadrantMask, u8& newMask, bool undig)
    {
        static const u8 tileTable[] =
        { 4, 9, 11, 10, 3, 8, 7, 13, 1, 5, 12, 14, 2, 16, 15, 6 };
        // find old mask
        u32 oldMask = 99;
        for (u32 i = 0; i < sizeof(tileTable) / sizeof(u8); ++i)
        {
            if (tileTable[i] == oldGID)
            {
                oldMask = i;
                break;
            }
        }
        ECHO_ASSERT(oldMask != 99, "invalid GID %d!", oldGID);

        // calculate new mask
        if (undig)
        {
            newMask = oldMask | quadrantMask;
        }
        else
        {
            newMask = oldMask & ~quadrantMask;
        }

        return tileTable[newMask];
    }

    void GameMapController::addHp(f32 val)
    {
        ECHO_ASSERT(val >= 0, "The Num. of the power is not positive!");

        ECHO_TRACE("HP", "Hp added by %d", u32(val));
        m_curHp += val;
        if (m_curHp > MAX_HP)
        {
            ECHO_TRACE_NOARG("HP", "Warning: Hp overflow.");
            m_curHp = MAX_HP;
        }

        AudioManager::getInstance()->playSE("heal");
    }

    bool GameMapController::reduceHp(f32 val)
    {
        ECHO_ASSERT(val >= 0, "The Num. of the power is not positive!");

        if (val > 1.0f)
        {
            ECHO_TRACE("HP", "Hp reduced by %d", u32(val));
            AudioManager::getInstance()->playSE("hero_hurt");
        }
        bool alive = true;
        m_curHp -= val;
        if (m_curHp <= MIN_HP)
        {
            ECHO_TRACE_NOARG("HP", "Warning: Hp underflow.");
            m_curHp = MIN_HP;
            alive = false;
        }
        return alive;
    }

    bool GameMapController::updateHp(f32 dt)
    {
        bool isAlive = true;
#ifdef ECHO_CHEAT
        ECHO_UNUSED(dt);
        isAlive = reduceHp(0);
#else
        isAlive = reduceHp(dt * DPS_COMMON);
#endif
        static const u32 hpThresholds[] = {0, 1, 10, 20, 30, 40, 50, 60, 70, 80, 90};
        for (u32 i = sizeof(hpThresholds) / sizeof(u32) - 1; i >= 0; --i)
        {
            if (m_curHp / MAX_HP * 100 >= hpThresholds[i])
            {
                updateHpBar(i);
                break;
            }
        }

        return isAlive;
    }

    void GameMapController::updateHpBar(u32 i)
    {
        if (i == m_curHpBar) return;

        if (i < m_curHpBar)
        {
            ECHO_TRACE("HP", "Hp bar reduced by %d", m_curHpBar - i);
            for (u32 j = i + 1; j <= m_curHpBar; ++j)
            {
                String hpBarName = ECHO_STR("UI_HpBar_%d", j);
                auto hpBarj = dynamic_cast<cocos2d::ui::ImageView*>(m_pUiController->getWidgetByName(hpBarName));
                ECHO_ASSERT(hpBarj, "Cannot get HpBar_%d", j);
                hpBarj->loadTexture("ui/Hp_0.png");
            }
            m_owner->playAnimation("ReduceHp", m_curHpBar, i);
        }
        else
        {
            ECHO_TRACE("HP", "Hp bar recovered by %d", i - m_curHpBar);
            for (u32 j = m_curHpBar + 1; j <= i; ++j)
            {
                String hpBarName = ECHO_STR("UI_HpBar_%d", j);
                auto hpBarj = dynamic_cast<cocos2d::ui::ImageView*>(m_pUiController->getWidgetByName(hpBarName));
                ECHO_ASSERT(hpBarj, "Cannot get HpBar_%d", j);
                hpBarj->loadTexture("ui/Hp_1.png");
            }
            m_owner->playAnimation("AddHp", m_curHpBar, i);
        }

        m_curHpBar = i;
    }

    cocos2d::Vec2 GameMapController::getRealPos(u32 x, u32 y)
    {
        return cocos2d::Vec2(SceneGameMap::MAP_OFFSET_X + SIZE_PER_BLOCK * x, SceneGameMap::MAP_OFFSET_Y + SIZE_PER_BLOCK * y);
    }

    cocos2d::Vec2 GameMapController::getRealPos(MapPoint pos)
    {
        return cocos2d::Vec2(SceneGameMap::MAP_OFFSET_X + SIZE_PER_BLOCK * pos.m_x, SceneGameMap::MAP_OFFSET_Y + SIZE_PER_BLOCK * pos.m_y);
    }

    bool GameMapController::checkHeroReach(u32 x, u32 y)
    {
        return x == m_heroCurPosX && y == m_heroCurPosY;
    }

    void GameMapController::checkEntityReached(f32 dt)
    {
        std::vector<Entity*> ents = m_owner->getCurEntities();
        for (auto curEnt : ents)
        {
            if (checkHeroReach(curEnt->getPosX(), curEnt->getPosY()))
            {
                ECHO_TRACE("GameMap", "Collided with %d, type=%s", curEnt->getUid(), Entity::typeToString(curEnt->getType()).c_str());
                switch (curEnt->getType())
                {
                case eET_Supply
                    :
                {
                    ECHO_TRACE("HP", "Collected supply %d with amount %d", curEnt->getUid(), curEnt->getAmount());
                    addHp(curEnt->getAmount());
                    curEnt->collect();
                    ++m_supplyCount;
                    m_realScore += 10;
                    break;
                }
                case eET_Key
                    :
                {
                    KeyAndDoor* key = dynamic_cast<KeyAndDoor*>(curEnt);
                    key->open(key);
                    break;
                }
                case eET_Boss
                    :
                {
                    ECHO_TRACE("Boss", "Reach boss: %d", curEnt->getUid());

                    Boss* theBoss = dynamic_cast<Boss*>(curEnt);
                    theBoss->getOwner()->collide(dt);
                    
                    break;
                }
                case eET_TreasureBox
                    :
                {
                    ECHO_TRACE("Treasure", "Reach treasurebox %d", curEnt->getUid());
                    auto treasure = dynamic_cast<TreasureBox*>(curEnt);
                    treasure->open();
                    break;
                }
                case eET_Bomb
                    :
                {
                    ECHO_TRACE("Treasure", "Reach bomb %d", curEnt->getUid());
                    auto bomb = dynamic_cast<Bomb*>(curEnt);
                    bomb->startQte();
                    break;
                }
                default
                    :
                {
                    ECHO_TRACE_NOARG("GameMap", "Untreated");
                    break;
                }
                }
            }
        }
    }

    u32 GameMapController::getMapHeight()
    {
        return MAP_HEIGHT;
    }

    u32 GameMapController::getMapWidth()
    {
        return MAP_WIDTH;
    }

    Echo::u32 GameMapController::getHeroCurPosX() const
    {
        return m_heroCurPosX;
    }

    Echo::u32 GameMapController::getHeroCurPosY() const
    {
        return m_heroCurPosY;
    }

    Echo::MapPoint GameMapController::getHeroCurPos() const
    {
        return MapPoint(m_heroCurPosX, m_heroCurPosY);
    }

    void GameMapController::addCoin(u32 amount)
    {
        ECHO_ASSERT(amount > 0, "amount < 0!");

        m_coinCount += amount;
    }

    SceneGameMap* GameMapController::getOwner() const
    {
        return m_owner;
    }

    void GameMapController::addBoss(BaseBossController* controller)
    {
        ECHO_ASSERT(controller, "BossController is nullptr!");
#ifdef COCOS2D_DEBUG
        for (auto itr : m_bosses)
        {
            ECHO_ASSERT(itr != controller, "Boss already exist!");
        }
#endif

        m_bosses.push_back(controller);
    }

    void GameMapController::removeBoss(BaseBossController* controller)
    {
        ECHO_ASSERT(controller, "BossController is nullptr!");

        bool found = false;
        for (auto itr = m_bosses.begin(); itr != m_bosses.end();)
        {
            if ((*itr) == controller)
            {
                found = true;
                BaseBossController* boss = (*itr);
                itr = m_bosses.erase(itr);
                CC_SAFE_DELETE(boss);
                break;
            }
            else
            {
                ++itr;
            }
        }

        if (!found)
        {
            ECHO_TRACE_NOARG("Boss", "Warning: boss does not exist!");
        }
    }

    Echo::u32 GameMapController::getBossCount()
    {
        return m_bosses.size();
    }

    void GameMapController::clearBosses()
    {
        while (!m_bosses.empty())
        {
            auto bossController = m_bosses.front();
            removeBoss(bossController);
        }
    }

    void GameMapController::updateBosses(f32 dt)
    {
        for (auto boss : m_bosses)
        {
            boss->update(dt);
        }
    }

    void GameMapController::setPause(bool val)
    {
        m_paused = val;
        if (val)
        {
            cocos2d::Director::getInstance()->getActionManager()->pauseTarget(getHeroSprite());
        }
        else
        {
            cocos2d::Director::getInstance()->getActionManager()->resumeTarget(getHeroSprite());
        }
    }

    void GameMapController::setInMiniGame(bool val)
    {
        ECHO_ASSERT(m_inMiniGame != val, "invalid");
        m_inMiniGame = val;
    }

    bool GameMapController::getPause()
    {
        return m_paused;
    }

    bool GameMapController::getInMiniGame()
    {
        return m_inMiniGame;
    }

    void GameMapController::gameOver(bool win)
    {
        m_paused = true;
        m_gameOver = true;

        // Statistic data
        u32 timeMin = u32(m_curTime) / 60;
        u32 timeSec = u32(m_curTime) % 60;
        u32 restHpPercent = u32(m_curHp * 100 / MAX_HP);

        u32 timeScore = timeMin * 60 + timeSec;
        u32 hpScore = restHpPercent;
        u32 supplyScore = m_supplyCount * 10;
        u32 treasureScore= m_treasureCount * 100 + m_coinCount;
        u32 totalScore = timeScore + hpScore + supplyScore + treasureScore;

        // get widget
        auto panel = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName("IMG_ClearPanel"));
        auto clearText = dynamic_cast<cocos2d::ui::ImageView*>(getUIController()->getWidgetByName("TXT_Clear"));

        auto LBL_Time_Min = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Time_Min"));
        auto LBL_Time_Sec = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Time_Sec"));
        auto LBL_Hp = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Hp"));
        auto LBL_Supply = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Supply"));
        auto LBL_Treasure = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Treasure"));

        auto LBL_Time_Score = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Time_Score"));
        auto LBL_Hp_Score = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Hp_Score"));
        auto LBL_Supply_Score = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Supply_Score"));
        auto LBL_Treasure_Score = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Treasure_Score"));
        auto LBL_Total_Score = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_Total_Score"));

        // set value
        LBL_Time_Min->setString(ECHO_STR("%d", timeMin));
        LBL_Time_Sec->setString(ECHO_STR("%d", timeSec));
        LBL_Hp->setString(ECHO_STR("%d", restHpPercent));
        LBL_Supply->setString(ECHO_STR("%d", m_supplyCount));
        LBL_Treasure->setString(ECHO_STR("%d", m_treasureCount));

        LBL_Time_Score->setString(ECHO_STR("%d", timeScore));
        LBL_Hp_Score->setString(ECHO_STR("%d", hpScore));
        LBL_Supply_Score->setString(ECHO_STR("%d", supplyScore));
        LBL_Treasure_Score->setString(ECHO_STR("%d", treasureScore));
        LBL_Total_Score->setString(ECHO_STR("%d", totalScore));

        m_owner->playAnimation("AddHp", 1, m_curHpBar);
        addRealScore(hpScore);

        panel->runAction(cocos2d::Sequence::create(
            cocos2d::DelayTime::create(3.0f),
            cocos2d::CallFunc::create([=]()
        {
            if (win)
            {
                ECHO_TRACE_NOARG("Flow", "You win!");

                panel->loadTexture("ui/StageClear.png");
                clearText->loadTexture("ui/Txt_StageClear.png");
                getUIController()->playUIAnimation("ClearPanel_Show_Win");

                AudioManager::getInstance()->playSE("win");
            }
            else
            {
                ECHO_TRACE_NOARG("Flow", "You lose!");

                panel->loadTexture("ui/GameOver.png");
                clearText->loadTexture("ui/Txt_GameOver.png");
                getUIController()->playUIAnimation("ClearPanel_Show_Lose");

                AudioManager::getInstance()->playSE("lose");
            }

            AudioManager::getInstance()->playBGM("", true);
            cocos2d::Director::getInstance()->getActionManager()->pauseTarget(getHeroSprite());
        }),
            NULL
            ));
    }

    cocos2d::Sprite* GameMapController::getHeroSprite()
    {
        if (!m_heroSprite)
        {
            auto heroNode = m_owner->getHeroNode();
            auto heroCom = dynamic_cast<cocostudio::ComRender*>(heroNode->getComponent("CCSprite"));
            m_heroSprite = dynamic_cast<cocos2d::Sprite*>(heroCom->getNode());
        }
        return m_heroSprite;
    }

    ArrowManager* GameMapController::getArrowManager() const
    {
        return m_arrowManager;
    }

    void GameMapController::updateScore(f32 dt)
    {
        if (m_curScoreCd > 0.0f)
        {
            m_curScoreCd -= dt;
        }
        else
        {
            if (m_realScore != m_dispScore)
            {
                m_curScoreCd = 0.5f / (m_realScore - m_dispScore - 0.5f);
                ++m_dispScore;
                ECHO_TRACE("Hp","disp=%d, real=%d, cd=%f", m_dispScore, m_realScore, m_curScoreCd);
                auto scoreLable = dynamic_cast<cocos2d::ui::TextAtlas*>(getUIController()->getWidgetByName("LBL_CurScore"));
                scoreLable->setString(ECHO_STR("%d", m_dispScore));
            }
        }
    }

    void GameMapController::addRealScore(u32 val)
    {
        m_realScore += val;
    }

    void GameMapController::addTreasureCount(u32 coin)
    {
        m_treasureCount += 1;
        m_coinCount += coin;
    }

    bool GameMapController::posValid(MapPoint pos)
    {
        return pos.m_x > 0 && pos.m_x < MAP_WIDTH && pos.m_y > 0 && pos.m_y < MAP_HEIGHT;
    }

    void GameMapController::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* /*event*/)
    {
        if (keyCode == cocos2d::EventKeyboard::KeyCode::KEY_BACK)
        {
            if (m_backCd < MATH_EPSILON)
            {
                m_backCd = 1.0f;
                ECHO_TRACE_NOARG("DEBUG", "Back pressed.");
                if (m_pausePanelShowed)
                {
                    this->BTN_Resume("");
                }
                else if (!m_paused)
                {
                    this->BTN_Pause("");
                }
            }
        }
    }

    void GameMapController::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* /*event*/)
    {
        ECHO_UNUSED(keyCode);
    }

}
