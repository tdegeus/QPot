/**
Version information.

\file QPot/version.hpp
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_VERSION_H
#define QPOT_VERSION_H

#include <string>
#include <algorithm>

#include "config.h"

/**
Current version.

Either:

-   Configure using CMake at install time. Internally uses::

        python -c "from setuptools_scm import get_version; print(get_version())"

-   Define externally using::

        -DQPOT_VERSION="`python -c "from setuptools_scm import get_version; print(get_version())"`"

    From the root of this project. This is what ``setup.py`` does.

Note that both ``CMakeLists.txt`` and ``setup.py`` will construct the version using ``setuptools_scm``.
Tip: use the environment variable ``SETUPTOOLS_SCM_PRETEND_VERSION``
to overwrite the automatic version.
*/
#ifndef QPOT_VERSION
#define QPOT_VERSION "@QPOT_VERSION@"
#endif

namespace QPot {

/**
Return version string, e.g.::


    "0.8.0"

\return std::string
*/
inline std::string version();

/**
Return versions of this library and of all of its dependencies.
The output is a list of strings, e.g.::

    "goosefem=0.7.0",
    "xtensor=0.20.1"
    ...

\return List of strings.
*/
inline std::vector<std::string> version_dependencies();

// --------------
// Implementation
// --------------

namespace detail {

    inline std::string unquote(const std::string& arg)
    {
        std::string ret = arg;
        ret.erase(std::remove(ret.begin(), ret.end(), '\"'), ret.end());
        return ret;
    }

}

inline std::string version()
{
    return detail::unquote(std::string(QUOTE(QPOT_VERSION)));
}

inline std::vector<std::string> version_dependencies()
{
    std::vector<std::string> ret;

    ret.push_back("goosefem=" + version());

    ret.push_back("xtensor=" +
        detail::unquote(std::string(QUOTE(XTENSOR_VERSION_MAJOR))) + "." +
        detail::unquote(std::string(QUOTE(XTENSOR_VERSION_MINOR))) + "." +
        detail::unquote(std::string(QUOTE(XTENSOR_VERSION_PATCH))));

    return ret;
}

} // namespace QPot

#endif
