/**
Use a static list of yielding positions.

\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_STATIC_HPP
#define QPOT_STATIC_HPP

#include <xtensor/xnoalias.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>

#include "config.h"
#include "version.hpp"

namespace QPot {

/**
Yielding positions for one particle.
*/
class Static {
public:
    Static() = default;

    /**
    Constructor.

    \param x Current position.
    \param x_yield Sequence of yield positions.
    */
    Static(double x, const xt::xtensor<double, 1>& x_yield);

    /**
    Yield positions.

    \return All yield positions [ntotal].
    */
    xt::xtensor<double, 1> yieldPosition() const;

    /**
    Slice yieldPosition()[start: stop].

    \param start Lower column bound.
    \param stop Upper column bound.
    \return Yield positions [stop - start].
    */
    xt::xtensor<double, 1> yieldPosition(size_t start, size_t stop) const;

    /**
    Slice yieldPosition()[i].

    \param i Column to select.
    \return Yield position.
    */
    double yieldPosition(size_t i) const;

    /**
    Customise proximity search region.

    \param distance Width of the region to consider as proximity (default: 10).
    */
    void setProximity(size_t distance);

    /**
    Update current position.

    \param x Current position.
    */
    void setPosition(double x);

    /**
    Yielding position at an offset left/right of the current position:
    -   offset > 0: ``yieldPosition()[current_index + offset]``
    -   offset < 0: ``yieldPosition()[current_index + offset + 1]``

    \param offset The offset.
    \return Yield position.
    */
    double nextYield(int offset) const;

    /**
    Yielding position left of the current position

        yieldPosition()[current_index]

    \return Yield position.
    */
    double currentYieldLeft() const;

    /**
    Yielding position right of the current position

        yieldPosition()[current_index + 1]

    \return Yield position.
    */
    double currentYieldRight() const;

    /**
    Yielding position at an offset left of the current position

        yieldPosition()[current_index - offset]

    \param offset The offset (same for all particles).
    \return Yield position.
    */
    double currentYieldLeft(size_t offset) const;

    /**
    Yielding position at an offset right of the current position

        yieldPosition()[current_index + 1 + offset]

    \param offset The offset (same for all particles).
    \return Yield position.
    */
    double currentYieldRight(size_t offset) const;

    /**
    Slice of yielding positions at an offset left/right of the current position

        yieldPosition()[current_index + left: current_index + right]

    \param left Offset left (same for all particles).
    \param right Offset right (same for all particles).
    \return Yield positions [right - left].
    */
    xt::xtensor<double, 1> currentYield(int left, int right) const;

    /**
    Index of the current minimum

        yieldPosition()[:, index] -> yielding position left
        yieldPosition()[:, index + 1] -> yielding position right

    \return Index.
    */
    size_t currentIndex() const;

    /**
    Check that the particle is at least ``n`` wells from the far-left.

    \param n Offset.
    \return boolean.
    */
    bool checkYieldBoundLeft(size_t n = 0) const;

    /**
    Check that the particle is at least ``n`` wells from the far-right.

    \param n Offset.
    \return boolean.
    */
    bool checkYieldBoundRight(size_t n = 0) const;

private:
    /**
    Initialisation.
    */
    void init();

private:
    size_t m_proximity = 10; ///< Size of neighbourhood to search first.
    xt::xtensor<double, 1> m_pos; ///< Yielding positions.
    size_t m_ntot; ///< Len(m_pos).
    size_t m_idx; ///< Current "index".
    double m_max; ///< Maximum yielding position ``== m_pos[0]``.
    double m_min; ///< Minimum yielding position ``== m_pos[-1]``.
    double m_left; ///< Current yielding position to the left ``== m_pos[m_idx]``.
    double m_right; ///< Current yielding position to the right ``== m_pos[m_idx + 1]``.
};

// --------------
// Implementation
// --------------

inline Static::Static(double x, const xt::xtensor<double, 1>& x_yield) : m_pos(x_yield)
{
    m_ntot = m_pos.size();
    m_proximity = std::min(m_proximity, m_ntot);

    m_min = m_pos(0);
    m_max = m_pos(m_pos.size() - 1);
    QPOT_ASSERT(x > m_min);
    QPOT_ASSERT(x < m_max);

    m_idx = std::lower_bound(m_pos.begin(), m_pos.end(), x) - m_pos.begin() - 1;
    m_left = m_pos(m_idx);
    m_right = m_pos(m_idx + 1);
}

inline xt::xtensor<double, 1> Static::yieldPosition() const
{
    return m_pos;
}

inline xt::xtensor<double, 1> Static::yieldPosition(size_t start, size_t stop) const
{
    QPOT_ASSERT(stop <= m_ntot);
    return xt::view(m_pos, xt::range(start, stop));
}

inline double Static::yieldPosition(size_t i) const
{
    QPOT_ASSERT(i < m_ntot);
    return m_pos(i);
}

inline xt::xtensor<double, 1> Static::currentYield(int left, int right) const
{
    QPOT_ASSERT(left <= 0);
    QPOT_ASSERT(right >= 0);
    QPOT_ASSERT(static_cast<int>(m_idx) + left >= 0);
    QPOT_ASSERT(static_cast<int>(m_idx) + right < static_cast<int>(m_ntot));
    return xt::view(m_pos, xt::range(m_idx + left, m_idx + right));
}

inline double Static::currentYieldLeft(size_t offset) const
{
    QPOT_ASSERT(offset <= m_idx);
    return m_pos(m_idx - offset);
}

inline double Static::currentYieldRight(size_t offset) const
{
    QPOT_ASSERT(m_idx + 1 + offset < m_ntot);
    return m_pos(m_idx + 1 + offset);
}

inline double Static::nextYield(int offset) const
{
    QPOT_ASSERT(offset != 0);

    if (offset < 0) {
        size_t shift = static_cast<size_t>(-offset);
        QPOT_ASSERT(shift - 1 <= m_idx);
        return m_pos(m_idx - shift + 1);
    }

    size_t shift = static_cast<size_t>(offset);
    QPOT_ASSERT(m_idx + shift < m_ntot);
    return m_pos(m_idx + shift);
}

inline void Static::setProximity(size_t proximity)
{
    m_proximity = std::min(proximity, m_ntot);
}

inline void Static::setPosition(double x)
{
    QPOT_ASSERT(x > m_min);
    QPOT_ASSERT(x < m_max);

    if (m_left < x && m_right >= x) {
        return;
    }

    size_t l = m_idx > m_proximity ? m_idx - m_proximity : 0;
    size_t r = std::min(m_idx + m_proximity, m_ntot - 1);

    if (m_pos(l) < x && m_pos(r) >= x) {
        m_idx = std::lower_bound(&m_pos(l), &m_pos(l) + r - l, x) - &m_pos(l) - 1 + l;
    }
    else {
        m_idx = std::lower_bound(m_pos.begin(), m_pos.end(), x) - m_pos.begin() - 1;
    }

    m_left = m_pos(m_idx);
    m_right = m_pos(m_idx + 1);
}

inline double Static::currentYieldLeft() const
{
    return m_left;
}

inline double Static::currentYieldRight() const
{
    return m_right;
}

inline size_t Static::currentIndex() const
{
    return m_idx;
}

inline bool Static::checkYieldBoundLeft(size_t n) const
{
    QPOT_ASSERT(n < m_ntot);
    return m_idx > n;
}

inline bool Static::checkYieldBoundRight(size_t n) const
{
    QPOT_ASSERT(n < m_ntot);
    return m_idx < m_ntot - n;
}

} // namespace QPot

#endif
