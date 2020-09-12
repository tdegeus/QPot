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
    Static(double x, const xt::xtensor<double,1>& yield);

    // Return yield positions
    xt::xtensor<double,1> yield() const;

    // Customise proximity search
    void setProximity(size_t proximity);

    // Update current position
    void setPosition(double x);

    // Get the yielding position left/right, based on the current position "x"
    double currentYieldLeft() const; // y[index]
    double currentYieldRight() const; // y[index + 1]

    // Get the index of the current minimum. Note:
    // - "index" : yielding position left
    // - "index + 1" : yielding position right
    size_t currentIndex() const;

private:

    // Initialise
    void init();

private:

    size_t m_proximity = 10; // neighbourhood to search first
    xt::xtensor<double,1> m_pos; // yielding positions
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

inline Static::Static(double x, const xt::xtensor<double,1>& yield) : m_pos(yield)
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

inline xt::xtensor<double,1> Static::yield() const
{
    return m_pos;
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

} // namespace QPot

#endif
