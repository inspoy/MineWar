//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_UTILITIES_ECHOMACROS_H__INCLUDED
#define ECHO_UTILITIES_ECHOMACROS_H__INCLUDED

#include "cocos2d.h"
#include "EchoUtl.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && COCOS2D_DEBUG)
#define ECHO_CHEAT
#else
#undef ECHO_CHEAT
#endif

// 取消下面一行注释可暂时取消作弊
// #undef ECHO_CHEAT

// 将cocos2d-x的log方法封装为一个宏
#ifdef COCOS2D_DEBUG

#define ECHO_TRACE( TAG, FORMAT, ... )\
    Echo::EchoUtl::logToFile(TAG, FORMAT, __VA_ARGS__); \

#define ECHO_TRACE_NOARG( TAG, MESSAGE )\
    Echo::EchoUtl::logToFile(TAG, MESSAGE);
#else
#define ECHO_TRACE( TAG, FORMAT, ... ) do {} while (0)
#define ECHO_TRACE_NOARG( TAG, MESSAGE ) do {} while (0)
#endif

// 实现功能较多的Assert宏，cocos2d-x提供的CCASSERT经测试无法断在指定行
#ifdef COCOS2D_DEBUG
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define ECHO_ASSERT( CONDITION, ... )\
    do\
    {\
        if(!(CONDITION))\
        {\
            ECHO_TRACE_NOARG("Assert", "===============CONDITION FAILURE==============="); \
            ECHO_TRACE("Assert", "COND: %s", #CONDITION); \
            ECHO_TRACE("Assert", "FILE: %s", __FILE__); \
            ECHO_TRACE("Assert", "LINE: %d", __LINE__); \
            ECHO_TRACE("Assert", "FUNC: %s", __FUNCTION__); \
            ECHO_TRACE_NOARG("Assert", "==============================================="); \
            abort(); \
        }\
    } while (0)
#else
#define ECHO_ASSERT( CONDITION, FORMAT, ... )\
do\
    {\
if(!(CONDITION))\
        {\
        ECHO_TRACE("Assert", "===============CONDITION FAILURE==============="); \
        ECHO_TRACE("Assert", "COND: %s", #CONDITION); \
        ECHO_TRACE("Assert", "FILE: %s", __FILE__); \
        ECHO_TRACE("Assert", "LINE: %d", __LINE__); \
        ECHO_TRACE("Assert", "FUNC: %s", __FUNCTION__); \
        ECHO_TRACE("Assert", "Message:"); \
        ECHO_TRACE("Assert", FORMAT, __VA_ARGS__); \
        ECHO_TRACE("Assert", "===============================================\n"); \
        __asm {int 3}; \
        }\
    } while (0)
#endif
#else
#define ECHO_ASSERT( CONDITION, FORMAT, ... ) do {} while (0)
#endif

#define ECHO_UNUSED(x) ((void)(&x))

// 单件模式
#define ECHO_DECLARE_SINGLETON(ClassName)\
public:\
    ~ClassName(); \
    static ClassName* getInstance()\
    {\
        ECHO_ASSERT(isSingletonCreated(), "Singleton %s has not been created!", #ClassName); \
        return m_instance; \
    }\
    static bool createSingleton()\
    {\
        ECHO_ASSERT(!isSingletonCreated(), "Singleton %s has already been created!", #ClassName); \
        m_instance = new ClassName(); \
        return true; \
    }\
    static bool destroySingleton()\
    {\
        ECHO_ASSERT(isSingletonCreated(), "Singleton %s has not been created!", #ClassName); \
        CC_SAFE_DELETE(m_instance); \
        return true; \
    }\
    static bool isSingletonCreated()\
    {\
        return m_instance != NULL; \
    }\
private:\
    ClassName(); \
    ClassName(const ClassName&) {}\
    ClassName& operator=(const ClassName&) {}\
    static ClassName* m_instance
#define ECHO_IMPLEMENT_SINGLETON(ClassName)\
    ClassName* ClassName::m_instance = NULL

#endif

// 流程控制
#define ECHO_BREAK_IF(cond)     if (cond) break
#define ECHO_BREAK_UNLESS(cond) if (!(cond)) break

// 简化函数路径
#define ECHO_END_GAME Echo::EchoUtl::endGame()

#define ECHO_STR(...) cocos2d::StringUtils::format(__VA_ARGS__)
