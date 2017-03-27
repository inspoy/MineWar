//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "Gameplay/Engine/AudioManager.h"
#include "CCFileUtils.h"

namespace Echo
{

    ECHO_IMPLEMENT_SINGLETON(AudioManager);

    AudioManager::AudioManager()
        : m_engine(NULL)
    {}

    AudioManager::~AudioManager()
    {}

    void AudioManager::init()
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        ECHO_TRACE_NOARG("Audio", "Init audio manager...");
#else
#error Unsupport platform!
#endif
        ConfigData cd("audio/config");
        String key;
        String path;
        u32 session;
        u32 no;
        m_assets.clear();
        for (session = 1;; ++session)
        {
            key = ECHO_STR("%d/1", session);
            if (cd.isKeyExist(key.stdStr))
            {
                for (no = 1;; ++no)
                {
                    key = ECHO_STR("%d/%d", session, no);
                    if (cd.isKeyExist(key.stdStr))
                    {
                        path = cd.getValueWithKey(key.stdStr);
                        AudioAsset asset;
                        asset.m_session = session;
                        asset.m_bgmOrSe = path.beginWith("BGM");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
                        asset.m_path = "audio/" + path.stdStr + ".ogg";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
                        if (asset.m_bgmOrSe)
                        {
                            asset.m_path = "audio/" + path.stdStr + ".mp3";
                        }
                        else
                        {
                            asset.m_path = "audio/" + path.stdStr + ".wav";
                        }
#endif
                        m_assets[session].push_back(asset);
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                break;
            }
        }
    }

    void AudioManager::preLoad(u32 session)
    {
        std::vector<AudioAsset>& assets = m_assets[session];
        for (auto asset : assets)
        {
            ECHO_ASSERT(asset.m_session == session, "");
            String path = cocos2d::FileUtils::getInstance()->fullPathForFilename(asset.m_path.stdStr);
            if (asset.m_bgmOrSe)
            {
                getEngine()->preloadBackgroundMusic(path.c_str());
            }
            else
            {
                getEngine()->preloadEffect(path.c_str());
            }
        }
    }

    void AudioManager::playBGM(String name, bool stop /*= false*/)
    {
        if (!stop)
        {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
            String fp = ECHO_STR("audio/BGM/%s.ogg", name.c_str());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
            String fp = ECHO_STR("audio/BGM/%s.mp3", name.c_str());
#endif
            fp = cocos2d::FileUtils::getInstance()->fullPathForFilename(fp.stdStr);

            m_engine->playBackgroundMusic(fp.c_str(), true);
        }
        else
        {
            m_engine->stopBackgroundMusic();
        }
    }

    void AudioManager::resumeBGM(bool pause /*= false*/)
    {
        if (!pause)
        {
            m_engine->resumeBackgroundMusic();
        }
        else
        {
            m_engine->pauseBackgroundMusic();
        }
    }

    u32 AudioManager::playSE(String name, bool loop /*= false*/)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        String fp = ECHO_STR("audio/SE/%s.ogg", name.c_str());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        String fp = ECHO_STR("audio/SE/%s.wav", name.c_str());
#endif
        fp = cocos2d::FileUtils::getInstance()->fullPathForFilename(fp.stdStr);
        return m_engine->playEffect(fp.c_str(), loop);
    }

    void AudioManager::stopSE(u32 seId /*= 0*/)
    {
        if (seId == 0)
        {
            m_engine->stopAllEffects();
        }
        else
        {
            m_engine->stopEffect(seId);
        }
    }

    CocosDenshion::SimpleAudioEngine* AudioManager::getEngine()
    {
        if (!m_engine)
        {
            m_engine = CocosDenshion::SimpleAudioEngine::getInstance();
        }
        return m_engine;
    }

}
