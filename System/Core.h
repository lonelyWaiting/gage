#pragma once

typedef unsigned int       uint;
typedef long long          int64;
typedef unsigned int       uint32;
typedef unsigned short     uint16;
typedef unsigned char      uint8;
typedef unsigned long long uint64;
typedef int                int32;
typedef short              int16;
typedef char               int8;

inline void dpf(const char* str, ...)
{
    va_list args;
    va_start(args, str);
    static char buffer[4096];
    vsprintf_s(buffer, 4096, str, args);
    OutputDebugStringA(buffer);
    va_end(args);
}

template <typename T> T align16(T value)
{
    return (value + 15) & ~0x0f;
}

#define SAFE_RELEASE(x) if(x) x->Release();

extern const int SIZEX;
extern const int SIZEY;

#include "Array.h"

