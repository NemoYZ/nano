#pragma once

#ifdef _MSC_VER
#ifdef _M_X64
#define BIT_SIZE 64
#else 
#define BIT_SIZE 32
#endif // _M_AMD64 
#elif (__GNUC__ || __clang__)
#ifdef __x86_64__
#define BIT_SIZE 64
#define BYTE_SIZE 8
#else //__i386__
#define BIT_SIZE 32
#define BYTE_SIZE 4
#endif //__x86_64__
#endif //__MSC_VER

#if BIT_SIZE == 64
#define BIT64
#else
#define BIT32
#endif //BIT_SIZE