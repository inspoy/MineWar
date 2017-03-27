//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#include "Utilities/EchoTypes.h"
#include "EchoUtl.h"
#include <iomanip>

namespace Echo
{

    String::String()
    {
        stdStr = "";
    }

    String::String(StdString str)
    {
        stdStr = str;
    }

    String::String(const char* str)
    {
        stdStr = str;
    }

    String::String(size_t len, char ch)
    {
        stdStr = StdString(len, ch);
    }

    String::String(StdString str, size_t index, size_t len)
    {
        stdStr = str.substr(index, len);
    }

    String::String(s32 val, u32 width, char fillWith/* = '0'*/)
    {
        std::stringstream ss;
        ss << std::setw(width) << std::setfill(fillWith) << val;
        stdStr = ss.str();
    }

    String::~String()
    {}

    const char* String::c_str()
    {
        return stdStr.c_str();
    }

    Echo::u32 String::toU32()
    {
#ifdef COCOS2D_DEBUG
        if (!isNumber())
        {
            ECHO_TRACE("String", "Warning: string `%s` is not a number.", stdStr.c_str());
            return 0;
        }
#endif // COCOS2D_DEBUG
        return atol(stdStr.c_str());
    }

    Echo::f32 String::toF32()
    {
#ifdef COCOS2D_DEBUG
        if (!isNumber())
        {
            ECHO_TRACE("String", "Warning: string `%s` is not a number.", stdStr.c_str());
            return 0;
        }
#endif // COCOS2D_DEBUG
        return f32(atof(stdStr.c_str()));
    }

    bool String::isNumber()
    {
        if (stdStr == "") return false;

        char chTemp;
        for (u32 i = 0; i < stdStr.length(); i++)
        {
            // strcpy(&chTemp, stdStr.substr(i, 1).c_str());
            chTemp = stdStr.at(i);
            if (!isDigit(chTemp))
            {
                return false;
            }
        }
        return true;
    }

    Echo::String String::uppercase()
    {
        StdString strTemp = "";
        char chTemp;
        for (u32 i = 0; i < stdStr.size(); i++)
        {
            strcpy(&chTemp, stdStr.substr(i, 1).c_str());
            strTemp += toUpper(chTemp);
        }
        stdStr = strTemp;
        return strTemp;
    }

    Echo::String String::lowercase()
    {
        StdString strTemp = "";
        char chTemp;
        for (u32 i = 0; i < stdStr.size(); i++)
        {
            strcpy(&chTemp, stdStr.substr(i, 1).c_str());
            strTemp += toLower(chTemp);
        }
        stdStr = strTemp;
        return strTemp;
    }

    String& String::operator=(const String& str)
    {
        stdStr = str.stdStr;
        return *this;
    }

    String& String::operator=(const StdString& str)
    {
        stdStr = str;
        return *this;
    }

    String& String::operator=(const char* str)
    {
        stdStr = str;
        return *this;
    }

    bool String::operator==(const String& str)
    {
        return stdStr == str.stdStr;
    }

    bool String::operator==(const StdString& str)
    {
        return stdStr == str;
    }

    bool String::operator==(const char* str)
    {
        return strcmp(stdStr.c_str(), str) == 0;
    }

    char String::operator[](u32 index)
    {
        return stdStr[index];
    }

    bool String::beginWith(String prefix)
    {
        return !(stdStr.compare(0, prefix.length(), prefix.stdStr));
    }

    bool String::endWith(String suffix)
    {
        if (stdStr.length() < suffix.length())
        {
            return false;
        }
        else
        {
            return !(stdStr.compare(stdStr.length() - suffix.length(), suffix.length(), suffix.stdStr));
        }
    }

    Echo::u32 String::length()
    {
        return stdStr.length();
    }

}