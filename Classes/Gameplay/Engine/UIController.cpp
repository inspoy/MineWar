//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/UIController.h"
#include "ui/UIWidget.h"
#include "ui/UIButton.h"
#include "ui/UIImageView.h"
#include "platform/CCFileUtils.h"
#include "cocostudio/CCActionManagerEx.h"
#include "ui/UIHelper.h"

namespace Echo
{

    UIController::UIController()
    {}

    UIController::~UIController()
    {}

    bool UIController::init()
    {
        m_curSceneName = "";
        m_apiFuncMap.clear();
        m_widgetList.clear();
        m_tagMap.clear();
        m_rootNode = NULL;
        return true;
    }

    bool UIController::initWithLayout(cocos2d::Node* uiRoot, String name)
    {
        ECHO_ASSERT(uiRoot, "uiRoot is Null!");

        m_apiFuncMap.clear();
        m_widgetList.clear();
        m_rootNode = uiRoot;
        m_curSceneName = name;
        readTagListFromFile(m_tagMap, name);
        for (auto itr : m_tagMap)
        {
            u32 tag = itr.second;
            String widgetName = itr.first;

            // For button
            if (widgetName.stdStr.substr(0, 3) == "BTN")
            {
                cocos2d::ui::Button* btn = dynamic_cast<cocos2d::ui::Button*>(cocos2d::ui::Helper::seekWidgetByTag((cocos2d::ui::Widget*)uiRoot, tag));
                if (btn && btn->isTouchEnabled())
                {
                    btn->addTouchEventListener(CC_CALLBACK_2(UIController::handleTouchEvent, this));
                    continue;
                }
            }
            else if (widgetName.stdStr.substr(0, 3) == "IMG")
            {
                cocos2d::ui::ImageView* img = dynamic_cast<cocos2d::ui::ImageView*>(cocos2d::ui::Helper::seekWidgetByTag((cocos2d::ui::Widget*)uiRoot, tag));
                if (img && img->isTouchEnabled())
                {
                    img->addTouchEventListener(CC_CALLBACK_2(UIController::handleTouchEvent, this));
                    continue;
                }
            }
        }
        return true;
    }

    void UIController::handleTouchEvent(cocos2d::Ref* pSender, cocos2d::ui::Widget::TouchEventType eventType)
    {
        auto btn = dynamic_cast<cocos2d::ui::Button*>(pSender);
        auto img = dynamic_cast<cocos2d::ui::ImageView*>(pSender);
        u32 tag = btn ? btn->getTag() : (img ? img->getTag() : 0);
        if (tag == 0)
        {
            ECHO_TRACE_NOARG("UI", "Handled unknown touch event.");
            return;
        }
        // get widget string with tag
        StdString uid = "Invalid";
        for (auto itr : m_widgetList)
        {
            if (itr.m_sceneName == m_curSceneName && itr.m_tag == tag)
            {
                uid = itr.m_uid.stdStr;
                break;
            }
        }
        // ECHO_TRACE("UI", "Handled Touch Event, scene is %s, uid is %s, type is %d", m_curSceneName.c_str(), uid.c_str(), eventType);
        if (m_apiFuncMap.find(uid) == m_apiFuncMap.end()) return;
        bool ret = false;
        // call the specific function
        switch (eventType)
        {
        case cocos2d::ui::Widget::TouchEventType::BEGAN:
            if (m_apiFuncMap[uid].m_press)
            {
                ECHO_TRACE("UI", "API Func %s called.", uid.c_str());
                ret = m_apiFuncMap[uid].m_pressFunc(uid);
            }
            break;
        case cocos2d::ui::Widget::TouchEventType::MOVED:
            break;
        case cocos2d::ui::Widget::TouchEventType::ENDED:
            if (m_apiFuncMap[uid].m_release)
            {
                ECHO_TRACE("UI", "API Func %s called.", uid.c_str());
                ret = m_apiFuncMap[uid].m_releaseFunc(uid);
            }
            break;
        case cocos2d::ui::Widget::TouchEventType::CANCELED:
            break;
        default:
            ECHO_ASSERT(false, "");
            break;
        }
#ifdef COCOS2D_DEBUG
        if (ret)
        {
            ECHO_TRACE("UI", "Button %d Clicked!", tag);
        }
#endif
    }

    void UIController::readTagListFromFile(std::map<StdString, u32>& tagList, String name)
    {
        tagList.clear();
        auto filePath = ECHO_STR("UITags_%s", name.c_str());
        ConfigData cd(filePath);
        for (auto itr : cd.m_data)
        {
            StdString widgetName = itr.first;
            u32 widgetTag = cd.getValueWithKey(widgetName).toU32();
            ECHO_ASSERT(tagList.find(widgetName) == tagList.end(), "Tag %d already appeared!", widgetTag);
            tagList[widgetName] = widgetTag;
            WidgetInfo wi(widgetName, name, widgetTag);
            m_widgetList.push_back(wi);
        }
    }

    void UIController::registerAPIFunction(ApiFunc func, StdString uid, bool pressOrRelease/* = false*/)
    {
        if (pressOrRelease)
        {
            //ECHO_ASSERT(m_apiFuncMap.find(uid) == m_apiFuncMap.end(), "This funcName \"%s\" has already been registered.", uid.c_str());

            m_apiFuncMap[uid].m_pressFunc = func;
            m_apiFuncMap[uid].m_press = true;
        }
        else
        {
            m_apiFuncMap[uid].m_releaseFunc = func;
            m_apiFuncMap[uid].m_release = true;
        }
    }

    void UIController::playUIAnimation(String name)
    {
        String jsonName = cocos2d::String::createWithFormat("%s_UI_1.json", m_curSceneName.c_str())->getCString();
        auto result = cocostudio::ActionManagerEx::getInstance()->playActionByName(jsonName.c_str(), name.c_str());
        ECHO_UNUSED(result);
        ECHO_ASSERT(result, "Cannot find animation %s", name.c_str());
    }

    Echo::u32 UIController::getTagByWidgetName(String name)
    {
        u32 retVal = 0;
        if (m_tagMap.find(name.stdStr) != m_tagMap.end())
        {
            retVal = m_tagMap[name.stdStr];
        }
        
        return retVal;
    }

    cocos2d::ui::Widget* UIController::getWidgetByTag(u32 tag)
    {
        return cocos2d::ui::Helper::seekWidgetByTag((cocos2d::ui::Widget*)m_rootNode, tag);
    }

    cocos2d::ui::Widget* UIController::getWidgetByName(String name)
    {
        return getWidgetByTag(getTagByWidgetName(name));
    }

}
