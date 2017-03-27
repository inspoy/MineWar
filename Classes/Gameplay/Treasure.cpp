//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Gameplay/Treasure.h"
#include "Precompiled.h"
#include "Scenes/GameMapController.h"
#include "Engine/AudioManager.h"
#include "Utilities/TimeUtl.h"

namespace Echo
{

    TreasureBox::TreasureBox()
        : m_opened(false)
        , m_needTap(100.0f)
        , m_curTap(50.0f)
        , m_touchListener(NULL)
        , m_treasureNode(NULL)
        , m_treasureBarNode(NULL)
    {}

    TreasureBox::~TreasureBox()
    {}

    void TreasureBox::update(f32 dt)
    {
        if (m_opened == true)
        {
            // update sprite
            auto boxNode = getTreasureNode();
            boxNode->setRotation((0.5f - m_curTap / m_needTap) * 180.0f);
            auto barNode = getTreasureBarNode();
            barNode->setPercent(m_curTap / m_needTap * 100.0f);

            m_curTap -= 20 * dt;

            if (m_curTap >= m_needTap || m_curTap < 0)
            {
                if (m_curTap >= m_needTap)
                {
                    ECHO_TRACE_NOARG("Treasure", "Get treasure success !");
                    m_gameMapController->addCoin(50);
                    m_gameMapController->getUIController()->playUIAnimation("TreasurePanel_Close_Win");
                    AudioManager::getInstance()->playSE("minigame_Clear");
                    f32 curTime = TimeUtl::getInstance()->getRunTime().toSec();
                    u32 coins = 100 / (curTime - m_beginTime);
                    m_gameMapController->addTreasureCount(coins);
                    m_gameMapController->addRealScore(coins + 100);
                }
                else
                {
                    ECHO_TRACE_NOARG("Treasure", "Get treasure Failed !");
                    m_gameMapController->getUIController()->playUIAnimation("TreasurePanel_Close_Lose");
                    AudioManager::getInstance()->playSE("minigame_Failed");
                }

                m_gameMapController->setInMiniGame(false);

                auto bgNode = dynamic_cast<cocos2d::Node*>(m_gameMapController->getUIController()->getWidgetByName("IMG_TreasurePanel"));
                cocos2d::Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(bgNode);

                this->collect();
            }
        }
    }

    void TreasureBox::open()
    {
        ECHO_TRACE_NOARG("Treasure", "Opening...");
        m_opened = true;
        m_beginTime = TimeUtl::getInstance()->getRunTime().toSec();

        m_touchListener = cocos2d::EventListenerTouchOneByOne::create();
        m_touchListener->onTouchBegan = CC_CALLBACK_2(TreasureBox::onTouchBegan, this);
        m_touchListener->onTouchMoved = CC_CALLBACK_2(TreasureBox::onTouchMoved, this);
        m_touchListener->onTouchEnded = CC_CALLBACK_2(TreasureBox::onTouchEnded, this);
        auto bgNode = dynamic_cast<cocos2d::Node*>(m_gameMapController->getUIController()->getWidgetByName("IMG_TreasurePanel"));
        cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(m_touchListener, bgNode);

        m_gameMapController->setInMiniGame(true);

        m_gameMapController->getUIController()->playUIAnimation("TreasurePanel_Show");
    }

    bool TreasureBox::onTouchBegan(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {
        ECHO_TRACE("Treasure", "Clicked, curTap = %d", u32(m_curTap));
        m_curTap += 7.0f;
        return true;
    }

    void TreasureBox::onTouchMoved(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {}

    void TreasureBox::onTouchEnded(cocos2d::Touch* /*touch*/, cocos2d::Event* /*event*/)
    {}

    cocos2d::Node* TreasureBox::getTreasureNode()
    {
        if (!m_treasureNode)
        {
            m_treasureNode = dynamic_cast<cocos2d::Node*>(m_gameMapController->getUIController()->getWidgetByName("IMG_Treasure_Trophy"));
            ECHO_ASSERT(m_treasureNode, "Cannot get treasure node!");
        }

        return m_treasureNode;
    }

    cocos2d::ui::LoadingBar* TreasureBox::getTreasureBarNode()
    {
        if (!m_treasureBarNode)
        {
            m_treasureBarNode = dynamic_cast<cocos2d::ui::LoadingBar*>(m_gameMapController->getUIController()->getWidgetByName("UI_TreasureBar"));
            ECHO_ASSERT(m_treasureBarNode, "Cannot get treasure bar node!");
        }

        return m_treasureBarNode;
    }

}