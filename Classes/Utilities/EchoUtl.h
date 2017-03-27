//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_UTILITIES_ECHOUTL_H__INCLUDED
#define ECHO_UTILITIES_ECHOUTL_H__INCLUDED

#include "Precompiled.h"

namespace Echo
{

    struct ConfigData
    {
        ConfigData()
        : m_dataCount(0)
        {
            m_data.clear();
        }

        ConfigData(String path)
        {
            readDataFromFile(path);
        }

        void readDataFromFile(String path);

        String getValueWithKey(String key);
        bool isKeyExist(String key);

        u32 m_dataCount;
        std::map<StdString, StdString> m_data;
    };

    class EchoUtl
    {
    public:
        static void logToFile(const char* tag, const char* format, ...);
        static void flushLog();
        static void endGame();

        static u32 strToInt(String arg);
        static f32 strToFloat(String arg);

    protected:
        static void nativeLog(const char* tag, const char *format, va_list args);
        static const u32 MAX_LOG_LENGTH;
        static const String LOG_FILE_PATH;
    };
}

#endif