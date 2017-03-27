//----------------------------------------------------------
//
// Echo Entertainment @ 2014
// All Rights Reserved
//
//----------------------------------------------------------

#ifndef ECHO_UTILITIES_ECHOTYPES_H__INCLUDED
#define ECHO_UTILITIES_ECHOTYPES_H__INCLUDED

#include <string>

namespace Echo
{

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    typedef unsigned char u8;
    typedef unsigned short u16;
    typedef unsigned long u32;
    typedef unsigned long long u64;
    
    typedef signed char s8;
    typedef signed short s16;
    typedef signed long s32;
    typedef signed long long s64;
    
    typedef float f32;
    typedef double f64;

    const u8 U8_MAX = 0xFF;
    const u16 U16_MAX = 0xFFFF;
    const u32 U32_MAX = 0xFFFFFFFF;

    const s8 S8_MAX = s8(127);
    const s8 S8_MIN = s8(-127 - 1);
    const s16 S16_MAX = s16(32767);
    const s16 S16_MIN = s16(-32767 - 1);
    const s32 S32_MAX = s32(2147483647);
    const s32 S32_MIN = s32(-2147483647 - 1);

    typedef std::string StdString;

    class String
    {
    public:
        // access
        const char* c_str();
        StdString stdStr;

    public:
        // constructor
        String();
        String(const StdString str);
        String(const char* str);
        String(size_t len, char ch);
        String(StdString str, size_t index, size_t len);
        String(s32 val, u32 width, char fillWith);
        ~String();
        
        // convert
        u32 toU32();
        f32 toF32();

        // tools
        u32 length();
        bool isNumber();
        String uppercase();
        String lowercase();
        bool beginWith(String prefix);
        bool endWith(String suffix);

        // operator
        String& operator=(const String& str);
        String& operator=(const StdString& str);
        String& operator=(const char* str);

        bool operator==(const String& str);
        bool operator==(const StdString& str);
        bool operator==(const char* str);

        char operator[](u32 index);

    private:
        inline bool isDigit(char& ch) { return((ch >= '0' && ch <= '9') || (ch == '.')); }
        inline char toUpper(char& ch) { return(ch >= 'a' && ch <= 'z') ? (ch - 'a' + 'A') : ch; }
        inline char toLower(char& ch) { return(ch >= 'A' && ch <= 'Z') ? (ch - 'A' + 'a') : ch; }
    };
#else
    #error "Unknown Platform!"
#endif

}
#endif