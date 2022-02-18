/**
Macros used in the library.

\file
\copyright Copyright 2020. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_CONFIG_H
#define QPOT_CONFIG_H

#include <stdexcept>
#include <string>

/**
\cond
*/
#define Q(x) #x
#define QUOTE(x) Q(x)

#define QPOT_ASSERT_IMPL(expr, file, line, function) \
    if (!(expr)) { \
        throw std::runtime_error( \
            std::string(file) + ":" + std::to_string(line) + " (" + std::string(function) + ")" + \
            ": assertion failed (" #expr ") \n\t"); \
    }
/**
\endcond
*/

/**
All assertions are implemented as:

    QPOT_ASSERT(...)

They can be enabled by:

    #define QPOT_ENABLE_ASSERT

(before including QPot).
The advantage is that:

-   File and line-number are displayed if the assertion fails.
-   QPot's assertions can be enabled/disabled independently from those of other libraries.

\throw std::runtime_error
*/
#ifdef QPOT_ENABLE_ASSERT
#define QPOT_ASSERT(expr) QPOT_ASSERT_IMPL(expr, __FILE__, __LINE__, __FUNCTION__)
#else
#define QPOT_ASSERT(expr)
#endif

/**
Intensive assertions (e.g. checking if input is sorted) are implemented as:

    QPOT_DEBUG(...)

They can be enabled by:

    #define QPOT_ENABLE_DEBUG

(before including QPot).

\throw std::runtime_error
*/
#ifdef QPOT_ENABLE_DEBUG
#define QPOT_DEBUG(expr) QPOT_ASSERT_IMPL(expr, __FILE__, __LINE__, __FUNCTION__)
#else
#define QPOT_DEBUG(expr)
#endif

/**
Assertions that cannot be disable.

\throw std::runtime_error
*/
#define QPOT_REQUIRE(expr) QPOT_ASSERT_IMPL(expr, __FILE__, __LINE__, __FUNCTION__)

/**
Keep track of potential energy landscape.
*/
namespace QPot {
}

#endif
