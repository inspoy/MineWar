//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Precompiled.h"
#include "EchoUtl.h"
#include <fstream>
#include "CCFileUtils.h"
#include "Utilities/TimeUtl.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include <jni.h>
#include "platform/android/jni/JniHelper.h"
#endif

namespace Echo
{
    
    const u32 EchoUtl::MAX_LOG_LENGTH = 16384;
    const String EchoUtl::LOG_FILE_PATH = "echoRunTime.log";

    void EchoUtl::logToFile(const char* tag, const char* format, ...)
    {
        if (!format) return;

        va_list args;
        va_start(args, format);
        nativeLog(tag, format, args);
        va_end(args);
    }

    void EchoUtl::nativeLog(const char* tag, const char *format, va_list args)
    {
        char buf[MAX_LOG_LENGTH];

        vsnprintf(buf, MAX_LOG_LENGTH - 3, format, args);

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        String filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LOG_FILE_PATH.stdStr);
        std::ofstream ofs(filePath.stdStr, std::ios::app);
        String timeStr = "[End]";
        if (TimeUtl::isSingletonCreated())
        {
            timeStr = ECHO_STR("[%s]:", TimeUtl::getInstance()->getRunTimeStr().c_str());
        }
        ofs << timeStr.stdStr;
        ofs<<"["<<tag<<"] - "<<buf<<std::endl;
        ofs.close();
        cocos2d::log(ECHO_STR("[%s] - %s", tag, buf).c_str());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        cocos2d::JniMethodInfo t;
        bool isHave = cocos2d::JniHelper::getStaticMethodInfo(t,
            "android/util/Log",
            "i",
            "(Ljava/lang/String;Ljava/lang/String;)I");
        if (isHave)
        {
            jstring jTitle = t.env->NewStringUTF("EchoDebug");
            jstring jMsg = t.env->NewStringUTF(
                buf);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, jTitle,
                jMsg);
            t.env->DeleteLocalRef(jTitle);
            t.env->DeleteLocalRef(jMsg);
        }
        else
        {
            cocos2d::CCLog("the jni method is not exits");
        }
        //! Running in android, cannot log to file
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        //! Running in ios, cannot log to file
#endif

    }

    void EchoUtl::flushLog()
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        String filePath = cocos2d::FileUtils::getInstance()->fullPathForFilename(LOG_FILE_PATH.stdStr);
        std::ofstream ofs(filePath.stdStr, std::ios::out);
        ofs << "===============MineWar Game Log===============" << std::endl;
        ofs.close();
#endif
    }

    void EchoUtl::endGame()
    {
        ECHO_TRACE_NOARG("Flow", "Game will exit...");

        // end DataEye in java side
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        cocos2d::JniMethodInfo info;
        if (cocos2d::JniHelper::getStaticMethodInfo(info, "com/echo/gravitation/AppActivity", "endGame", "()V"))
        {
            info.env->CallStaticVoidMethod(info.classID, info.methodID);
        }
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        
#endif

        // end the director
        cocos2d::Director::getInstance()->end();
    }

    Echo::u32 EchoUtl::strToInt(String arg)
    {
        return atol(arg.c_str());
    }

    Echo::f32 EchoUtl::strToFloat(String arg)
    {
        return f32(atof(arg.c_str()));
    }

    String ConfigData::getValueWithKey(String key)
    {
        ECHO_ASSERT(isKeyExist(key), "Cannot get value with key: %s", key.c_str());

        String ret;
        for (auto itr : m_data)
        {
            if (itr.first == key.stdStr)
            {
                ret = itr.second;
                break;
            }
        }
        return ret;
    }

    bool ConfigData::isKeyExist(String key)
    {
        bool res = false;
        for (auto itr : m_data)
        {
            if (itr.first == key.stdStr)
            {
                res = true;
                break;
            }
        }

        return res;
    }

    void ConfigData::readDataFromFile(String path)
    {
        auto pFileUtl = cocos2d::FileUtils::getInstance();
        String fullPath = pFileUtl->fullPathForFilename(path.stdStr + ".csv");
        ECHO_ASSERT(pFileUtl->isFileExist(fullPath.stdStr), "File: %s does not exist.", fullPath.c_str());
        String fileData = pFileUtl->getStringFromFile(fullPath.stdStr);
        std::stringstream ifs(fileData.stdStr);
        String line;
        while (!ifs.eof())
        {
            std::getline(ifs, line.stdStr);
            if (line[0] == '#' || line[0] == ',') continue; // skip comment line and blank line
            if (line == "" || line == "\r" || line == "\n" || line == "\r\n") continue; // skip blank line

            StdString ss(line.stdStr);
            StdString keyStr;
            StdString valueStr;
            s32 commaPos = ss.find(',');
            ECHO_ASSERT(commaPos != -1, "comma not exist!");
            keyStr = ss.substr(0, commaPos);
            valueStr = ss.substr(commaPos + 1);
            if (valueStr.find('\r') != -1) // remove \r at the end of the string
            {
                valueStr = valueStr.substr(0, valueStr.length() - 1);
            }

            m_dataCount += 1;
            m_data[keyStr] = valueStr;
        }
    }

}
