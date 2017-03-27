#include "Precompiled.h"
#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "Utilities/EchoUtl.h"
#include "Utilities/TimeUtl.h"
#include "Gameplay/Engine/GameLogic.h"
#include "Gameplay/Engine/MetaDataHelper.h"
#include "Gameplay/Engine/GameSceneFactory.h"
#include "Gameplay/Engine/GameObject/GameScene.h"
#include "Gameplay/Engine/AudioManager.h"

USING_NS_CC;

#define MAX_FRAME 60

AppDelegate::AppDelegate()
{
    Echo::EchoUtl::flushLog();
    Echo::TimeUtl::createSingleton();
    Echo::GameLogic::createSingleton();
    Echo::MetaDataHelper::createSingleton();
    Echo::GameSceneFactory::createSingleton();
    Echo::AudioManager::createSingleton();
}

AppDelegate::~AppDelegate() 
{
    auto director = Director::getInstance();
    director->getScheduler()->unscheduleAllForTarget(Echo::TimeUtl::getInstance());
    director->getScheduler()->unscheduleAllForTarget(Echo::GameLogic::getInstance());

    Echo::GameLogic::destroySingleton();
    Echo::MetaDataHelper::destroySingleton();
    Echo::GameSceneFactory::destroySingleton();
    Echo::AudioManager::getInstance()->getEngine()->end();
    Echo::AudioManager::destroySingleton();
    Echo::TimeUtl::destroySingleton();
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

bool AppDelegate::applicationDidFinishLaunching()
{
    ECHO_TRACE_NOARG("Flow", "Game Starting...");

    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview)
    {
        glview = GLViewImpl::create("MineWar");
        director->setOpenGLView(glview);
    }

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    // 设置窗口尺寸
    glview->setFrameSize(450.0f, 800.0f);
    // glview->setFrameSize(720.0f, 1280.0f);
#endif
    // 分辨率适应，添加黑边以适应非16:9屏幕
    glview->setDesignResolutionSize(720.0f, 1280.0f, ResolutionPolicy::SHOW_ALL);

#ifdef COCOS2D_DEBUG
    // turn on display FPS
    director->setDisplayStats(true);
#else
    // turn off display FPS
    director->setDisplayStats(false);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / MAX_FRAME);

    // read meta data
    Echo::MetaDataHelper::getInstance()->readMetaData();

    // init game logic
    Echo::GameLogic::getInstance()->init();
    director->getScheduler()->scheduleUpdateForTarget(Echo::GameLogic::getInstance(), 0, false);
    director->getScheduler()->scheduleUpdateForTarget(Echo::TimeUtl::getInstance(), 0, false);

    // init audio manager
    Echo::AudioManager::getInstance()->init();

    // create init scene
    auto scene = Echo::GameLogic::getInstance()->switchScene("SceneTitle", true)->getC2dScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
