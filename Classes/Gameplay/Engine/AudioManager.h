//----------------------------------------------------------
//
// Echo Entertainment @ 2015
// All Rights Reserved
//
//----------------------------------------------------------

#pragma once

#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "Precompiled.h"

namespace Echo
{

    struct AudioAsset
    {
        u32 m_session;
        bool m_bgmOrSe;
        String m_path;
    };

    class AudioManager
    {
    public:
        ECHO_DECLARE_SINGLETON(AudioManager);
    public:
        void init();
        void preLoad(u32 session);

        void playBGM(String name, bool stop = false);
        void resumeBGM(bool pause = false);
        u32 playSE(String name, bool loop = false);
        void stopSE(u32 seId = 0);

        CocosDenshion::SimpleAudioEngine* getEngine();

    private:
        CocosDenshion::SimpleAudioEngine* m_engine;
        std::map<u32, std::vector<AudioAsset> > m_assets;
    };

}