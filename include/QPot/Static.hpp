/*

(c - MIT) T.W.J. de Geus (Tom) | www.geus.me | github.com/tdegeus/QPot

*/

#ifndef QPOT_STATIC_HPP
#define QPOT_STATIC_HPP

#include "config.h"

namespace QPot {

class Static
{
public:

    Static() = default;

    // Constructor
    template <class T>
    Static(double x, const T& yield);

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

template <class T>
Static::Static(double x, const T& yield)
{
    // Copy input
    m_pos = yield;
    m_ntot = m_pos.size();

    // set proximity search distance
    m_proximity = std::min(m_proximity, m_ntot);

    // register minimum and maximum positions for each particle
    m_min = m_pos(0);
    m_max = m_pos(m_pos.size() - 1);
    QPOT_ASSERT(x > m_min);
    QPOT_ASSERT(x < m_max);

    // find current index and current yield positions
    m_idx = std::lower_bound(m_pos.begin(), m_pos.end(), x) - m_pos.begin() - 1;
    m_left = m_pos(m_idx);
    m_right = m_pos(m_idx + 1);
}

void Static::setProximity(size_t proximity)
{
    m_proximity = std::min(proximity, m_ntot);
}

void Static::setPosition(double x)
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

double Static::currentYieldLeft() const
{
    return m_left;
}

double Static::currentYieldRight() const
{
    return m_right;
}

size_t Static::currentIndex() const
{
    return m_idx;
}

} // namespace QPot

#endif
