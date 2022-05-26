#ifndef __PROZAC_MACRO_H__
#define __PROZAC_MACRO_H__

#include <cassert>
#include <iostream>
#include <prozac/util.h>

#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#define PROZAC_LIKELY(x) __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#define PROZAC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
#define PROZAC_LIKELY(x) (x)
#define PROZAC_UNLIKELY(x) (x)
#endif

/// 断言宏封装
#define PROZAC_ASSERT(x)                                        \
    if (PROZAC_UNLIKELY(!(x)))                                  \
    {                                                          \
        std::cout << "ASSERTION: " #x                          \
                  << "\nbacktrace:\n"                          \
                  << prozac::BacktraceToString(100, 2, "    "); \
        assert(x);                                             \
    }

/// 断言宏封装
#define PROZAC_ASSERT2(x, w)                                    \
    if (PROZAC_UNLIKELY(!(x)))                                  \
    {                                                          \
        std::cout << "ASSERTION: " #x                          \
                  << "\n"                                      \
                  << w                                         \
                  << "\nbacktrace:\n"                          \
                  << prozac::BacktraceToString(100, 2, "    "); \
        assert(x);                                             \
    }

#endif