//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_GAMEPLAY_UICONTROLLER_H__INCLUDED
#define ECHO_GAMEPLAY_UICONTROLLER_H__INCLUDED

#include "cocos2d.h"
#include "Precompiled.h"
#include "ui/UIWidget.h"

namespace Echo
{
    struct WidgetInfo
    {
        WidgetInfo(){}
        WidgetInfo(String uid, String sceneName, u32 tag)
            : m_uid(uid)
            , m_sceneName(sceneName)
            , m_tag(tag)
        {}
        String m_uid;
        String m_sceneName;
        u32 m_tag;
    };

    typedef std::function< bool( String uid ) > ApiFunc;

    struct ApiFuncInfo
    {
        ApiFuncInfo()
        : m_press(false)
        , m_release(false)
        {}
        bool    m_press;
        ApiFunc m_pressFunc;

        bool    m_release;
        ApiFunc m_releaseFunc;
    };

    class UIController
    {
    public:
        UIController();
        ~UIController();
        bool init();
        bool initWithLayout(cocos2d::Node* uiRoot, String name);
        void handleTouchEvent(cocos2d::Object* obj, cocos2d::ui::Widget::TouchEventType eventType);
        void playUIAnimation(String name);
        u32  getTagByWidgetName(String name);
        cocos2d::ui::Widget* getWidgetByTag(u32 tag);
        cocos2d::ui::Widget* getWidgetByName(String name);

        /**
        func: the callback function
        funcName: the widget name defined in UITags.txt
        */
        void registerAPIFunction(ApiFunc func, StdString funcName, bool pressOrRelease = false);

    private:
        void readTagListFromFile(std::map<StdString, u32>& tagList, String name);

    private:
        String m_curSceneName;
        std::map<StdString, ApiFuncInfo> m_apiFuncMap;
        std::vector<WidgetInfo> m_widgetList;
        std::map<StdString, u32> m_tagMap;
        cocos2d::Node* m_rootNode;
    };
}

#endif