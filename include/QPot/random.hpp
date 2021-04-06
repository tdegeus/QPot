/**
Wrapper around xtensor/xrandom.hpp

\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_RANDOM_HPP
#define QPOT_RANDOM_HPP

#include <xtensor/xtensor.hpp>
#include <xtensor/xrandom.hpp>

#include "config.h"
#include "version.hpp"

namespace QPot {

/**
Some wrappers of xtensor's random, to allow reconstruction from Python.
*/
namespace random {

/**
Set the random seed.

\param arg The seed.
*/
template <class T>
inline void seed(T arg)
{
    xt::random::seed(arg);
};

/**
Return a function to generate a matrix (rank 2 array) of ones.

\param scale Scale factor.
*/
inline std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)>
UniformList(double scale = 1.0)
{
    return [=](std::array<size_t, 2> shape) {
        return xt::eval(scale * xt::ones<double>(shape)); };
};

/**
Return a function to generate a matrix (rank 2 array) of random number,
taken from a uniform distribution between 0 and 1.

\param scale Scale factor.
*/
inline std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)>
RandList(double scale = 1.0)
{
    return [=](std::array<size_t, 2> shape) {
        return xt::eval(scale * xt::random::rand<double>(shape)); };
};

/**
Return a function to generate a matrix (rank 2 array) of random numbers,
taken from a Gamma distribution.

\param a Shape of the Weibull distribution.
\param b Scale of the Weibull distribution.
*/
inline std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)>
WeibullList(double a = 1.0, double b = 1.0)
{
    return [=](std::array<size_t, 2> shape) {
        return xt::eval(xt::random::weibull<double>(shape, a, b)); };
};

/**
Return a function to generate a matrix (rank 2 array) of random numbers,
taken from a gamma distribution.

\param alpha Shape of the gamma distribution.
\param beta Scale of the gamma distribution.
*/
inline std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)>
GammaList(double alpha = 1.0, double beta = 1.0)
{
    return [=](std::array<size_t, 2> shape) {
        return xt::eval(xt::random::gamma<double>(shape, alpha, beta)); };
};

} // namespace random
} // namespace QPot

#endif
