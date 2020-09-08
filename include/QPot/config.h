/*

(c - MIT) T.W.J. de Geus (Tom) | www.geus.me | github.com/tdegeus/QPot

*/

#ifndef QPOT_CONFIG_H
#define QPOT_CONFIG_H

#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xnoalias.hpp>

#ifdef QPOT_ENABLE_ASSERT

    #define QPOT_ASSERT(expr) \
        QPOT_ASSERT_IMPL(expr, __FILE__, __LINE__)

    #define QPOT_ASSERT_IMPL(expr, file, line) \
        if (!(expr)) { \
            throw std::runtime_error( \
                std::string(file) + ':' + std::to_string(line) + \
                ": assertion failed (" #expr ") \n\t"); \
        }

#else

    #define QPOT_ASSERT(expr)

#endif

#define QPOT_VERSION_MAJOR 0
#define QPOT_VERSION_MINOR 0
#define QPOT_VERSION_PATCH 1

#define QPOT_VERSION_AT_LEAST(x, y, z) \
    (QPOT_VERSION_MAJOR > x || (QPOT_VERSION_MAJOR >= x && \
    (QPOT_VERSION_MINOR > y || (QPOT_VERSION_MINOR >= y && \
                                QPOT_VERSION_PATCH >= z))))

#define QPOT_VERSION(x, y, z) \
    (QPOT_VERSION_MAJOR == x && \
     QPOT_VERSION_MINOR == y && \
     QPOT_VERSION_PATCH == z)

#endif
