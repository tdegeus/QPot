/**
Defines used in the library.

\file QPot/config.h
\copyright Copyright 2020. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_CONFIG_H
#define QPOT_CONFIG_H

/**
\cond
*/
#define Q(x) #x
#define QUOTE(x) Q(x)

#define QPOT_ASSERT_IMPL(expr, file, line) \
    if (!(expr)) { \
        throw std::runtime_error( \
            std::string(file) + ':' + std::to_string(line) + \
            ": assertion failed (" #expr ") \n\t"); \
    }
/**
\endcond
*/

/**
All assertions are implementation as::

    QPOT_ASSERT(...)

They can be enabled by::

    #define QPOT_ENABLE_ASSERT

(before including QPot).
The advantage is that:

-   File and line-number are displayed if the assertion fails.
-   QPot's assertions can be enabled/disabled independently from those of other libraries.

\throw std::runtime_error
*/
#ifdef QPOT_ENABLE_ASSERT
#define QPOT_ASSERT(expr) QPOT_ASSERT_IMPL(expr, __FILE__, __LINE__)
#else
#define QPOT_ASSERT(expr)
#endif

/**
Assertions that cannot be disable.

\throw std::runtime_error
*/
#define QPOT_REQUIRE(expr) QPOT_REQUIRE_IMPL(expr, __FILE__, __LINE__)

#endif
