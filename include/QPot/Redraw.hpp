/*

(c - MIT) T.W.J. de Geus (Tom) | www.geus.me | github.com/tdegeus/QPot

*/

#ifndef QPOT_REDRAW_HPP
#define QPOT_REDRAW_HPP

#include "config.h"

namespace QPot {

class RedrawList
{
public:

    RedrawList() = default;

    // Constructor
    template <class E, class F>
    RedrawList(
        const E& x, // current positions, number of particle derived from it
        F function_to_draw_yield_distances,
        size_t ntotal = 1000, // #yield-positions to keep in memory
        size_t nbuffer = 300, // #yield-positions to buffer when shifting left/right
        size_t noffset = 20); // #yield-positions at which to redraw maximally
                              // (allows grouping of redraws for several particles)

    // Return yield positions:
    xt::xtensor<double, 2> yield() const; // current list
    xt::xtensor<double, 2> yield(int left, int right) const; // around position: i + left: i + right

    // Customise proximity search region
    void setProximity(size_t distance);

    // Update current positions
    template <class E>
    void setPosition(const E& x);

    // Get the yielding positions left/right, based on the current positions "x"
    xt::xtensor<double, 1> currentYieldLeft() const; // y[:, index]
    xt::xtensor<double, 1> currentYieldRight() const; // y[:, index + 1]

    // Get the index of the current minima. Note:
    // - "index" : yielding positions left
    // - "index + 1" : yielding positions right
    xt::xtensor<long, 1> currentIndex() const;

    // Output raw data
    xt::xtensor<double, 2> raw_val() const;
    xt::xtensor<double, 2> raw_pos() const;
    xt::xtensor<size_t, 1> raw_idx() const;
    xt::xtensor<long, 1> raw_idx_t() const;

    // Restore raw data
    void restore(
        const xt::xtensor<double, 2>& val,
        const xt::xtensor<double, 2>& pos,
        const xt::xtensor<size_t, 1>& idx,
        const xt::xtensor<long, 1>& idx_t);

private:

    // Number of points
    size_t m_N;

    // Buffer settings
    size_t m_ntot; // number of yield positions to keep in memory at all times
    size_t m_nbuf; // number of yield positions to buffer
    size_t m_noff; // number of yield positions at which to shift maximally

    // Search settings
    size_t m_proximity = 10; // size of neighbourhood to search first

    // Yield positions
    xt::xtensor<double, 2> m_val; // drawn random values
    xt::xtensor<double, 2> m_pos; // yielding positions = cumsum(m_val, axis=1) + init
                                 // ('init' is computed during initialisation and redraw)
    xt::xtensor<size_t, 1> m_idx; // current "index": since the last shift
    xt::xtensor<long, 1> m_idx_t; // current "index": total up to the last shift (can be negative)
    xt::xtensor<double, 1> m_max; // maximum yielding positions
    xt::xtensor<double, 1> m_min; // minimum yielding positions
    xt::xtensor<double, 1> m_left; // current yielding positions to the left == m_pos[:, m_idx]
    xt::xtensor<double, 1> m_right; // current yielding positions to the right == m_pos[:, m_idx + 1]

    // Function to (re)draw yield positions
    std::function<xt::xtensor<double, 2>(std::vector<size_t>)> m_draw;
};

// --------------
// Implementation
// --------------

template <class E, class F>
inline RedrawList::RedrawList(const E& x, F draw, size_t ntotal, size_t nbuffer, size_t noffset)
    : m_N(x.size()), m_ntot(ntotal), m_nbuf(nbuffer), m_noff(noffset), m_draw(draw)
{
    QPOT_ASSERT(x.dimension() == 1);
    QPOT_ASSERT(m_nbuf <= m_ntot);
    QPOT_ASSERT(m_noff <= m_nbuf);

    m_proximity = std::min(m_proximity, m_ntot);

    // draw yield distance (times two!), and convert to yield positions
    m_val = m_draw({m_N, m_ntot});
    m_pos = xt::cumsum(m_val, 1);

    // shift such that the mean of "x" is in the middle of the potential energy landscape
    auto shift = xt::mean(m_pos, 1) - xt::mean(x);
    for (size_t p = 0; p < m_N; ++p) {
        xt::view(m_pos, p, xt::all()) -= shift(p);
    }

    // register minimum and maximum positions for each particle
    m_min = xt::view(m_pos, xt::all(), 0);
    m_max = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);
    QPOT_ASSERT(xt::all(x > m_min));
    QPOT_ASSERT(xt::all(x < m_max));

    // allocate current index
    m_idx = xt::empty<decltype(m_idx)::value_type>({m_N});
    m_idx_t = xt::zeros<decltype(m_idx_t)::value_type>({m_N});
    m_left = xt::empty<decltype(m_left)::value_type>({m_N});
    m_right = xt::empty<decltype(m_right)::value_type>({m_N});

    // compute current index (updates "m_idx" only, "m_idx_t" is only changed at redraw)
    for (size_t p = 0; p < m_N; ++p) {
        size_t i = std::lower_bound(&m_pos(p,0), &m_pos(p,0) + m_pos.shape(1), x(p)) - &m_pos(p,0) - 1;
        m_idx(p) = i;
        m_left(p) = m_pos(p, i);
        m_right(p) = m_pos(p, i + 1);
    }
}

inline xt::xtensor<double, 2> RedrawList::yield() const
{
    return m_pos;
}

inline xt::xtensor<double, 2> RedrawList::yield(int left, int right) const
{
    QPOT_ASSERT(left <= 0);
    QPOT_ASSERT(right >= 0);
    xt::xtensor<double, 2> ret = xt::empty<double>({m_N, static_cast<size_t>(right - left)});
    for (size_t p = 0; p < m_N; ++p) {
        QPOT_ASSERT(static_cast<int>(m_idx(p)) + left >= 0);
        QPOT_ASSERT(static_cast<int>(m_idx(p)) + right < static_cast<int>(m_ntot));
        xt::view(ret, p, xt::all()) = xt::view(m_pos, p, xt::range(m_idx(p) + left, m_idx(p) + right));
    }
    return ret;
}

inline void RedrawList::setProximity(size_t proximity)
{
    m_proximity = proximity;
}

template <class E>
inline void RedrawList::setPosition(const E& x)
{
    QPOT_ASSERT(x.size() == m_N);

    // extend right
    // ------------

    if (xt::any(x >= m_max))
    {
        // get rows for which a redraw is needed
        // use the opportunity to redraw multiple rows
        xt::xtensor<size_t, 1> index = xt::flatten_indices(xt::argwhere(
            x >= xt::view(m_pos, xt::all(), m_ntot - m_noff)));

        // allocate new yield distances (times two!)
        xt::xtensor<double, 2> y = xt::empty<double>({index.size(), m_ntot});

        // apply buffer: insert previously drawn values
        xt::view(y, xt::all(), xt::range(0, m_nbuf)) =
            xt::view(m_val, xt::keep(index), xt::range(m_ntot - m_nbuf, m_ntot));

        // draw yield distances (times two!)
        xt::view(y, xt::all(), xt::range(m_nbuf, m_ntot)) =
            m_draw({index.size(), m_ntot - m_nbuf});

        // store yield distances
        xt::view(m_val, xt::keep(index), xt::all()) = y;

        // update yield positions
        xt::view(y, xt::all(), 0) = xt::view(m_pos, xt::keep(index), m_ntot - m_nbuf);
        xt::view(m_pos, xt::keep(index), xt::all()) = xt::cumsum(y, 1);

        // update current index
        xt::view(m_idx_t, xt::keep(index)) += m_ntot - m_nbuf;
        xt::view(m_idx, xt::keep(index)) -= m_ntot - m_nbuf;

        // register minimum and maximum yield positions for each particle
        xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
        xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);
    }

    // extend left
    // -----------

    if (xt::any(x <= m_min))
    {
        // get rows for which a redraw is needed
        // use the opportunity to redraw multiple rows
        xt::xtensor<size_t, 1> index = xt::flatten_indices(xt::argwhere(
            x <= xt::view(m_pos, xt::all(), m_noff)));

        // allocate new yield distances (times two!)
        xt::xtensor<double, 2> y = xt::empty<double>({index.size(), m_ntot});

        // apply buffer: insert previously drawn values
        xt::view(y, xt::all(), xt::range(m_ntot - m_nbuf, m_ntot)) =
            xt::view(m_val, xt::keep(index), xt::range(0, m_nbuf));

        // draw yield distances (times two!)
        xt::view(y, xt::all(), xt::range(0, m_ntot - m_nbuf)) =
            m_draw({index.size(), m_ntot - m_nbuf});

        // store yield distances
        xt::view(m_val, xt::keep(index), xt::all()) = y;

        // update yield positions
        xt::view(y, xt::all(), 0) = xt::view(m_pos, xt::keep(index), m_nbuf) - xt::sum(y, 1);
        xt::view(m_pos, xt::keep(index), xt::all()) = xt::cumsum(y, 1);

        // update current index
        xt::view(m_idx_t, xt::keep(index)) -= m_ntot - m_nbuf;
        xt::view(m_idx, xt::keep(index)) += m_ntot - m_nbuf;

        // register minimum and maximum yield positions for each particle
        xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
        xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);
    }

    // update index
    // ------------

    QPOT_ASSERT(xt::all(x > m_min));
    QPOT_ASSERT(xt::all(x < m_max));

    using index_type = decltype(m_idx)::value_type;
    using position_type = typename E::value_type;

    for (size_t p = 0; p < m_N; ++p)
    {
        index_type i = m_idx(p);
        position_type xp = x(p);

        if (m_left(p) < xp && m_right(p) >= xp) {
            continue;
        }

        index_type l = i > m_proximity ? i - m_proximity : 0;
        index_type r = std::min(i + m_proximity, m_ntot - 1);

        if (m_pos(p, l) < xp && m_pos(p, r) >= xp) {
            i = std::lower_bound(&m_pos(p,l), &m_pos(p,l) + r - l, xp) - &m_pos(p,l) - 1 + l;
        }
        else {
            i = std::lower_bound(&m_pos(p,0), &m_pos(p,0) + m_pos.shape(1), xp) - &m_pos(p,0) - 1;
        }

        m_idx(p) = i;
        m_left(p) = m_pos(p, i);
        m_right(p) = m_pos(p, i + 1);
    }
}

inline xt::xtensor<double, 1> RedrawList::currentYieldLeft() const
{
    return m_left;
}

inline xt::xtensor<double, 1> RedrawList::currentYieldRight() const
{
    return m_right;
}

inline xt::xtensor<long, 1> RedrawList::currentIndex() const
{
    return m_idx_t + xt::cast<long>(m_idx);
}

inline xt::xtensor<double, 2> RedrawList::raw_val() const
{
    return m_val;
}

inline xt::xtensor<double, 2> RedrawList::raw_pos() const
{
    return m_pos;
}

inline xt::xtensor<size_t, 1> RedrawList::raw_idx() const
{
    return m_idx;
}

inline xt::xtensor<long, 1> RedrawList::raw_idx_t() const
{
    return m_idx_t;
}

inline void RedrawList::restore(
    const xt::xtensor<double, 2>& val,
    const xt::xtensor<double, 2>& pos,
    const xt::xtensor<size_t, 1>& idx,
    const xt::xtensor<long, 1>& idx_t)
{
    m_val = val;
    m_pos = pos;
    m_idx = idx;
    m_idx_t = idx_t;

    m_min = xt::view(m_pos, xt::all(), 0);
    m_max = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);

    for (size_t p = 0; p < m_N; ++p) {
        m_left(p) = m_pos(p, m_idx(p));
        m_right(p) = m_pos(p, m_idx(p) + 1);
    }
}

} // namespace QPot

#endif
