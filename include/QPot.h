/**
\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_H
#define QPOT_H

#include <algorithm>
#include <stdexcept>
#include <string>

/**
\cond
*/
#define Q(x) #x
#define QUOTE(x) Q(x)

#define QPOT_WARNING_IMPL(message, file, line, function) \
    std::cout << std::string(file) + ":" + std::to_string(line) + " (" + std::string(function) + \
                     ")" + ": " message ") \n\t";

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
All warnings specific to the Python API are implemented as:

    QPOT_WARNING_PYTHON(...)

They can be enabled by:

    #define QPOT_ENABLE_WARNING_PYTHON
*/
#ifdef QPOT_ENABLE_WARNING_PYTHON
#define QPOT_WARNING_PYTHON(message) QPOT_WARNING_IMPL(message, __FILE__, __LINE__, __FUNCTION__)
#else
#define QPOT_WARNING_PYTHON(message)
#endif

/**
Current version.

Either:

-   Configure using CMake at install time. Internally uses::

        python -c "from setuptools_scm import get_version; print(get_version())"

-   Define externally using::

        -DQPOT_VERSION="`python -c "from setuptools_scm import get_version; print(get_version())"`"

    From the root of this project. This is what ``setup.py`` does.

Note that both ``CMakeLists.txt`` and ``setup.py`` will construct the version using
``setuptools_scm``. Tip: use the environment variable ``SETUPTOOLS_SCM_PRETEND_VERSION`` to
overwrite the automatic version.
*/
#ifndef QPOT_VERSION
#define QPOT_VERSION "@PROJECT_VERSION@"
#endif

/**
Function to find items in an increase range, and to store this range in chunks.
*/
namespace QPot {

namespace detail {

inline std::string unquote(const std::string& arg)
{
    std::string ret = arg;
    ret.erase(std::remove(ret.begin(), ret.end(), '\"'), ret.end());
    return ret;
}

} // namespace detail

/**
Return version string, e.g.:

    "0.8.0"

\return std::string
*/
inline std::string version()
{
    return detail::unquote(std::string(QUOTE(QPOT_VERSION)));
}

/**
Return versions of this library and of all of its dependencies.
The output is a list of strings, e.g.:

    "qpot=0.8.0",
    "xtensor=0.20.1"
    ...

\return List of strings.
*/
inline std::vector<std::string> version_dependencies()
{
    std::vector<std::string> ret;

    ret.push_back("qpot=" + version());

    ret.push_back(
        "xtensor=" + detail::unquote(std::string(QUOTE(XTENSOR_VERSION_MAJOR))) + "." +
        detail::unquote(std::string(QUOTE(XTENSOR_VERSION_MINOR))) + "." +
        detail::unquote(std::string(QUOTE(XTENSOR_VERSION_PATCH))));

#ifdef XSIMD_VERSION_MAJOR
    ret.push_back(
        "xsimd=" + detail::unquote(std::string(QUOTE(XSIMD_VERSION_MAJOR))) + "." +
        detail::unquote(std::string(QUOTE(XSIMD_VERSION_MINOR))) + "." +
        detail::unquote(std::string(QUOTE(XSIMD_VERSION_PATCH))));
#endif

#ifdef XTL_VERSION_MAJOR
    ret.push_back(
        "xtl=" + detail::unquote(std::string(QUOTE(XTL_VERSION_MAJOR))) + "." +
        detail::unquote(std::string(QUOTE(XTL_VERSION_MINOR))) + "." +
        detail::unquote(std::string(QUOTE(XTL_VERSION_PATCH))));
#endif

#if defined(XTENSOR_PYTHON_VERSION_MAJOR)
    ret.push_back(
        "xtensor-python=" + detail::unquote(std::string(QUOTE(XTENSOR_PYTHON_VERSION_MAJOR))) +
        "." + detail::unquote(std::string(QUOTE(XTENSOR_PYTHON_VERSION_MINOR))) + "." +
        detail::unquote(std::string(QUOTE(XTENSOR_PYTHON_VERSION_PATCH))));
#endif

    std::sort(ret.begin(), ret.end(), std::greater<std::string>());

    return ret;
}

namespace iterator {

/**
Return index of the first element in the range [first, last) such that `element < value` is `false`
(i.e. greater or equal to), or last if no such element is found.

Compared to the default function, this function allows for a guess of the index and a proximity
search around. This could be efficient for finding items in large arrays.

\param first Iterator defining the beginning of the range to examine (e.g. `a.begin()`).
\param last Iterator defining the end of the range to examine (e.g. `a.end()`)
\param value Value to find.
\param guess Guess of the index where to find the value.
\param proximity Size of the proximity search around `guess` (use `0` to disable proximity search).
\return The index of `value` (i.e. `a[index] < value <= a[index + 1]`).
*/
template <class It, class T, class R = size_t>
inline R
lower_bound(const It first, const It last, const T& value, R guess = 0, size_t proximity = 10)
{
    if (proximity == 0) {
        return std::lower_bound(first, last, value) - first - 1;
    }

    if (*(first + guess) < value && *(first + guess + 1) >= value) {
        return guess;
    }

    size_t l = guess > proximity ? guess - proximity : 0;
    size_t r = std::min(guess + proximity, static_cast<size_t>(last - first - 1));

    if (*(first + l) < value && *(first + r) >= value) {
        return std::lower_bound(first + l, first + r, value) - first - 1;
    }
    else {
        return std::lower_bound(first, last, value) - first - 1;
    }
}

} // namespace iterator

namespace inplace {

/**
Similar to `lower_bound` but on the last axis of an nd-array (e.g. per row of a rank 2 matrix).

\param matrix The matrix defining a range per row.
\param value The value to find (per row).
\param index Initial guess on `index` (updated).
\param proximity Size of the proximity search around `guess` (use `0` to disable proximity search).
*/
template <class T, class V, class R>
inline void lower_bound(const T& matrix, const V& value, R& index, size_t proximity = 10)
{
    QPOT_ASSERT(value.dimension() == matrix.dimension() - 1);
    QPOT_ASSERT(value.dimension() == index.dimension());

    auto nd = value.dimension();
    auto stride = matrix.shape(nd);
    auto n = value.size();

#ifdef QPOT_ENABLE_ASSERT
    for (decltype(nd) i = 0; i < nd; ++i) {
        QPOT_ASSERT(matrix.shape(i) == value.shape(i));
        QPOT_ASSERT(matrix.shape(i) == index.shape(i));
    }
#endif

    for (decltype(n) i = 0; i < n; ++i) {
        index.flat(i) = iterator::lower_bound(
            &matrix.flat(i * stride),
            &matrix.flat(i * stride) + stride,
            value.flat(i),
            index.flat(i),
            proximity);
    }
}

/**
Update the chunk of a cumsum computed and stored in chunks.

\param cumsum The current chunk of the cumsum (updated).
\param delta The 'diff's of the next chunk in the cumsum.
\param shift The shift per row.
*/
template <class V, class I>
inline void cumsum_chunk(V& cumsum, const V& delta, const I& shift)
{
    QPOT_ASSERT(cumsum.dimension() >= 1);
    QPOT_ASSERT(cumsum.dimension() == delta.dimension());

    if (delta.size() == 0) {
        return;
    }

    size_t dim = cumsum.dimension();
    size_t n = cumsum.shape(dim - 1);
    size_t ndelta = delta.shape(dim - 1);

    for (size_t i = 0; i < shift.size(); ++i) {

        if (shift.flat(i) == 0) {
            continue;
        }

        auto* d = &delta.flat(i * ndelta);
        auto* c = &cumsum.flat(i * n);

        if (shift.flat(i) > 0) {
            QPOT_ASSERT(shift.flat(i) <= n);
            QPOT_ASSERT(ndelta >= shift.flat(i));
            size_t nadd = static_cast<size_t>(shift.flat(i));
            size_t nkeep = n - nadd;
            auto offset = *(c + n - 1);
            std::copy(c + n - nkeep, c + n, c);
            std::copy(d, d + nadd, c + nkeep);
            *(c + nkeep) += offset;
            std::partial_sum(c + nkeep, c + n, c + nkeep);
        }
        else {
            QPOT_ASSERT(-shift.flat(i) < n);
            QPOT_ASSERT(ndelta > -shift.flat(i));
            size_t nadd = static_cast<size_t>(-shift.flat(i));
            size_t nkeep = n - nadd;
            auto offset = *(c);
            std::copy(c, c + nkeep, c + nadd);
            std::copy(d, d + nadd + 1, c);
            std::partial_sum(c, c + nadd + 1, c);
            offset -= *(c + nadd);
            std::transform(c, c + nadd + 1, c, [&](auto& v) { return v + offset; });
        }
    }
}

} // namespace inplace

/**
Iterating on the last axis of an nd-array (e.g. per row of a rank 2 matrix):
Return index of the first element in the range [first, last) such that `element < value` is `false`
(i.e. greater or equal to), or last if no such element is found.

This function allows for a guess of the index and a proximity search around.
This could be efficient for finding items in large arrays.

\param matrix The matrix defining a range per row.
\param value The value to find (per row).
\param index Initial guess on `index`.
\param proximity Size of the proximity search around `guess` (use `0` to disable proximity search).
\return Same shape as `index`.
*/
template <class T, class V, class R>
inline R lower_bound(const T& matrix, const V& value, const R& index, size_t proximity = 10)
{
    R ret = index;
    inplace::lower_bound(matrix, value, ret, proximity);
    return ret;
}

/**
Iterating on the last axis of an nd-array (e.g. per row of a rank 2 matrix):
Return index of the first element in the range [first, last) such that `element < value` is `false`
(i.e. greater or equal to), or last if no such element is found.

\param matrix The matrix defining a range per row.
\param value The value to find (per row).
\return Same shape as `value`.
*/
template <class T, class V, class R>
inline R lower_bound(const T& matrix, const V& value)
{
    R ret = xt::zeros<typename R::value_type>(value.shape());
    inplace::lower_bound(matrix, value, ret, 0);
    return ret;
}

/**
\brief Update the chunk of a cumsum computed and stored in chunks.

\section example Example

Consider a full array:

    da = np.random.random(N)
    a = np.cumsum(a)

With chunk settings:

    n = ...  # size of each new chunk
    nbuffer = ... # number of items to buffer

The the first chunk:

    chunk = np.copy(a[:n + nbuffer])
    nchunk = n + nbuffer
    istart = np.array(0)

Then, moving right:

    QPot.cumsum_chunk_inplace(chunk, da[istart + nchunk : istart + nchunk + n], n)
    istart += n

Or, moving left:

    QPot.cumsum_chunk_inplace(chunk, da[istart - n : istart + 1], -n)
    istart -= n

\param cumsum The current chunk of the cumsum.
\param delta The 'diff's of the next chunk in the cumsum.
\param shift The shift per row.
\return Same shape as `cumsum`.
*/
template <class V, class I>
inline V cumsum_chunk(const V& cumsum, const V& delta, const I& shift)
{
    V ret = cumsum;
    inplace::cumsum_chunk(ret, delta, shift);
    return ret;
}

} // namespace QPot

#endif
