/*

(c - MIT) T.W.J. de Geus (Tom) | www.geus.me | github.com/tdegeus/QPot

*/

#ifndef QPOT_STATIC_HPP
#define QPOT_STATIC_HPP

#include "config.h"
#include <xtensor/xio.hpp>

namespace QPot {

class Static
{
public:

    Static() = default;

    // Constructor
    Static(double x, const xt::xtensor<double, 1>& yield);

    // Return yield positions:
    xt::xtensor<double, 1> yield() const; // all
    xt::xtensor<double, 1> yield(size_t start, size_t stop) const; // y[left: right]
    double yield(size_t i) const; // y[index]

    // Customise proximity search region
    void setProximity(size_t distance);

    // Update current position
    void setPosition(double x);

    // Get the yielding position left/right, based on the current position "x"
    double currentYieldLeft() const; // y[current_index]
    double currentYieldRight() const; // y[current_index + 1]
    double currentYieldLeft(size_t offset) const; // y[current_index - offset]
    double currentYieldRight(size_t offset) const; // y[current_index + 1 - offset]
    xt::xtensor<double, 1> currentYield(int left, int right) const; // y[current_index + left: current_index + right]

    // Get the index of the current minimum. Note:
    // - "current_index" : yielding position left
    // - "current_index + 1" : yielding position right
    size_t currentIndex() const;

    // Check that the particle is "n" wells from the far-left/right
    bool checkYieldBoundLeft(size_t n = 0) const;
    bool checkYieldBoundRight(size_t n = 0) const;

private:

    // Initialise
    void init();

private:

    size_t m_proximity = 10; // size of neighbourhood to search first
    xt::xtensor<double, 1> m_pos; // yielding positions
    size_t m_ntot; // len(m_pos)
    size_t m_idx; // current "index"
    double m_max; // maximum yielding position == m_pos[0]
    double m_min; // minimum yielding position == m_pos[-1]
    double m_left; // current yielding position to the left == m_pos[m_idx]
    double m_right; // current yielding position to the right == m_pos[m_idx + 1]
};

// --------------
// Implementation
// --------------

inline Static::Static(double x, const xt::xtensor<double, 1>& yield) : m_pos(yield)
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

inline xt::xtensor<double, 1> Static::yield() const
{
    return m_pos;
}

inline xt::xtensor<double, 1> Static::yield(size_t start, size_t stop) const
{
    QPOT_ASSERT(stop <= m_ntot);
    return xt::view(m_pos, xt::range(start, stop));
}

inline double Static::yield(size_t i) const
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
    QPOT_ASSERT(m_idx + offset < m_ntot);
    return m_pos(m_idx + 1 + offset);
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
