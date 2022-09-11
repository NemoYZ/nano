#pragma once

#include <functional>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "system.h"

namespace nano {

template<typename T>
struct hash_value {
};

//murmurhash2
//https://sites.google.com/site/murmurhash/
#ifdef BIT32
uint32_t murmurhash2 (const void* key, int len, unsigned int seed);

#define HASH_FUNCTION_INTEGRAL(Type)                \
template<>                                          \
struct hash_value<Type> {                           \
    uint32_t operator()(Type v) const noexcept {    \
        char buf[4]{0};                             \
        snprintf(buf, sizeof buf, "%u", v);         \
        return murmurhash2(buf, sizeof buf, 0);     \
    }                                               \
}
#else
uint64_t murmurhash2(const void* key, int len, unsigned int seed);

#define HASH_FUNCTION_INTEGRAL(Type)                \
template<>                                          \
struct hash_value<Type> {                           \
    uint64_t operator()(Type v) const noexcept {    \
        char buf[8]{0};                             \
        unsigned long long tmp = v;                 \
        snprintf(buf, sizeof buf, "%llu", tmp);     \
        return murmurhash2(buf, sizeof buf, 0);     \
    }                                               \
}
#endif //BIT32

HASH_FUNCTION_INTEGRAL(bool);

HASH_FUNCTION_INTEGRAL(char);

HASH_FUNCTION_INTEGRAL(signed char);

HASH_FUNCTION_INTEGRAL(unsigned char);

//HASH_FUNCTION_INTEGRAL(wchar_t);

//HASH_FUNCTION_INTEGRAL(char16_t);

//HASH_FUNCTION_INTEGRAL(char32_t);

HASH_FUNCTION_INTEGRAL(short);

HASH_FUNCTION_INTEGRAL(unsigned short);

HASH_FUNCTION_INTEGRAL(int);

HASH_FUNCTION_INTEGRAL(unsigned int);

HASH_FUNCTION_INTEGRAL(long);

HASH_FUNCTION_INTEGRAL(unsigned long);

HASH_FUNCTION_INTEGRAL(long long);

HASH_FUNCTION_INTEGRAL(unsigned long long);

#ifdef BIT32
#define HASH_RESULT_TYPE uint32_t
#else
#define HASH_RESULT_TYPE uint64_t
#endif //BIT32

template<>
struct hash_value<float> {
    HASH_RESULT_TYPE operator()(float v) const noexcept {
        unsigned int tmp = *reinterpret_cast<int*>(&v);
        return hash_value<unsigned int>()(tmp);
    }
};

template<>
struct hash_value<double> {
    HASH_RESULT_TYPE operator()(double v) const noexcept {
        unsigned long long tmp = *reinterpret_cast<unsigned long long*>(&v);
        return hash_value<unsigned long long>()(tmp);
    }
};

template<>
struct hash_value<long double> {
    HASH_RESULT_TYPE operator()(long double v) const noexcept {
        return murmurhash2(reinterpret_cast<const char*>(&v), sizeof(v), 0);
    }
};

template<>
struct hash_value<const char*> {
    HASH_RESULT_TYPE operator()(const char* str) const noexcept {
        return murmurhash2(str, strlen(str), 0);
    };
};

template<>
struct hash_value<const signed char*> {
    HASH_RESULT_TYPE operator()(const signed char* str) const noexcept {
        return hash_value<const char*>()(reinterpret_cast<const char*>(str));
    };
};

template<>
struct hash_value<const unsigned char*> {
    HASH_RESULT_TYPE operator()(const unsigned char* str) const noexcept {
        return hash_value<const char*>()(reinterpret_cast<const char*>(str));
    };
};

template<typename T>
struct hash_value<T*> {
    HASH_RESULT_TYPE operator()(T* ptr) const noexcept {
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        return hash_value<uintptr_t>()(addr);
    };
};

#undef HASH_RESULT_TYPE
#undef HASH_FUNCTION

} //namespace nano