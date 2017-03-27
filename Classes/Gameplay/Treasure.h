//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_TREASURE_H__INCLUDED
#define ECHO_GAMEPLAY_TREASURE_H__INCLUDED

#include "Precompiled.h"
#include "Item.h"
#include "ui\UILoadingBar.h"

namespace Echo
{
    
    class TreasureBox : public Entity
    {
    public:
        TreasureBox();
        ~TreasureBox();

        void update(f32 dt);
        void open();

    protected:

        bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
        void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
        cocos2d::Node* getTreasureNode();
        cocos2d::ui::LoadingBar* getTreasureBarNode();

    private:
        bool m_opened;

        f32 m_needTap;
        f32 m_curTap;
        f32 m_beginTime;

        cocos2d::EventListenerTouchOneByOne* m_touchListener;
        cocos2d::Node* m_treasureNode;
        cocos2d::ui::LoadingBar* m_treasureBarNode;
    };

}

#endif