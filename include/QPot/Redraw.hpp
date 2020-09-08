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

    // Constructor.
    template <class F>
    RedrawList(const xt::xtensor<double,1>& x, F function_to_draw_distances);

    // Customise parameters to control redrawing left/right.
    void setBuffer(size_t ntotal, size_t nbuffer, size_t noffset);

    // Customise proximity search
    void setProximity(size_t proximity);

    // Update current positions.
    void setPosition(const xt::xtensor<double,1>& x);

    // Get the yielding positions left/right, based on the current positions "x".
    xt::xtensor<double,1> currentYieldLeft() const; // y[:, index]
    xt::xtensor<double,1> currentYieldRight() const; // y[:, index + 1]

    // Get the index of the current yielding positions. Note:
    // - "index" : yielding positions left
    // - "index + 1" : yielding positions right
    xt::xtensor<long,1> currentIndex() const;

    // Output raw data.
    xt::xtensor<double,1> raw_x() const;
    xt::xtensor<double,2> raw_val() const;
    xt::xtensor<double,2> raw_pos() const;
    xt::xtensor<size_t,1> raw_idx() const;
    xt::xtensor<long,1> raw_idx_t() const;

private:

    // Initialise.
    void init();

    // Compute current "index".
    void updateIndex(); // all particles
    void updateIndex(size_t p); // particle "p" only

    // (Check to) redraw left/right.
    void extendRight();
    void extendLeft();

private:

    // Number of points.
    size_t m_N;

    // Buffer settings.
    size_t m_ntot = 1000; // number of yield positions to keep in memory at all times
    size_t m_nbuf = 300; // number of yield positions to buffer
    size_t m_noff = 20; // number of yield positions at which to shift maximally

    // Search settings
    size_t m_proximity = 10; // neighbourhood to search first

    // Current positions.
    xt::xtensor<double,1> m_x;

    // Yield positions.
    xt::xtensor<double,2> m_val; // drawn random values
    xt::xtensor<double,2> m_pos; // yielding ositions = cumsum(m_val, axis=1) + init
                                 // ('init' is set during initialisation and redraw)
    xt::xtensor<double,1> m_max; // maximum yielding position
    xt::xtensor<double,1> m_min; // minimum yielding position
    xt::xtensor<double,1> m_left; // current yielding position to the left == m_pos[:, m_idx]
    xt::xtensor<double,1> m_right; // current yielding position to the right == m_pos[:, m_idx + 1]

    // Current "index".
    xt::xtensor<size_t,1> m_idx; // since the last shift
    xt::xtensor<long,1> m_idx_t; // total up to the last shift

    // Function to (re)draw positions.
    std::function<xt::xtensor<double,2>(std::initializer_list<size_t>)> m_draw;
};

// --------------
// Implementation
// --------------

template <class F>
RedrawList::RedrawList(const xt::xtensor<double,1>& x, F draw)
    : m_N(x.size()), m_x(x), m_draw(draw)
{
    this->init();
}

void RedrawList::setBuffer(size_t ntotal, size_t nbuffer, size_t noffset)
{
    QPOT_ASSERT(nbuffer <= ntotal);
    QPOT_ASSERT(noffset <= nbuffer);
    m_ntot = ntotal;
    m_nbuf = nbuffer;
    m_noff = noffset;
    this->init();
}

void RedrawList::setProximity(size_t proximity)
{
    m_proximity = proximity;
}

void RedrawList::init()
{
    // set proximity search distance
    m_proximity = std::min(m_proximity, m_ntot);

    // draw yield distance (times two!)
    m_val = m_draw({m_N, m_ntot});

    // convert to yield positions
    m_pos = xt::cumsum(m_val, 1);

    // shift such that zero is in the middle
    auto mean = xt::mean(m_pos, 1);
    for (size_t p = 0; p < m_N; ++p) {
        xt::view(m_pos, p, xt::all()) -= mean(p);
    }

    // register minimum and maximum positions for each particle
    xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
    xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);
    QPOT_ASSERT(xt::all(m_x > m_min));
    QPOT_ASSERT(xt::all(m_x < m_max));

    // allocate current index
    m_idx = xt::empty<size_t>({m_N});
    m_idx_t = xt::zeros<int>({m_N});
    m_left = xt::empty<double>({m_N});
    m_right = xt::empty<double>({m_N});

    // compute current index (updates "m_idx" only, "m_idx_t" is only changed at redraw)
    for (size_t p = 0; p < m_N; ++p) {
        double x = m_x(p);
        size_t i = std::lower_bound(&m_pos(p,0), &m_pos(p,0) + m_pos.shape(1), x) - &m_pos(p,0) - 1;
        m_idx(p) = i;
        m_left(p) = m_pos(p, i);
        m_right(p) = m_pos(p, i + 1);
    }
}

void RedrawList::updateIndex(size_t p)
{
    size_t i = m_idx(p);
    double x = m_x(p);

    if (m_left(p) < x && m_right(p) >= x) {
        return;
    }

    size_t l = i > m_proximity ? i - m_proximity : 0;
    size_t r = std::min(i + m_proximity, m_ntot - 1);

    if (m_pos(p, l) < x && m_pos(p, r) >= x) {
        i = std::lower_bound(&m_pos(p,l), &m_pos(p,l) + r - l, x) - &m_pos(p,l) - 1 + l;
    }
    else {
        i = std::lower_bound(&m_pos(p,0), &m_pos(p,0) + m_pos.shape(1), x) - &m_pos(p,0) - 1;
    }

    m_idx(p) = i;
    m_left(p) = m_pos(p, i);
    m_right(p) = m_pos(p, i + 1);
}

void RedrawList::updateIndex()
{
    QPOT_ASSERT(xt::all(m_x > m_min));
    QPOT_ASSERT(xt::all(m_x < m_max));

    for (size_t p = 0; p < m_N; ++p) {
        updateIndex(p);
    }
}

void RedrawList::setPosition(const xt::xtensor<double,1>& x)
{
    QPOT_ASSERT(m_x.size() == m_N);
    xt::noalias(m_x) = x;
    this->extendRight();
    this->extendLeft();
    this->updateIndex();
}

void RedrawList::extendRight()
{
    // return if no redraw is needed
    if (!xt::any(m_x >= m_max)) {
        return;
    }

    // get rows for which a redraw is needed
    // use the opportunity to redraw multiple rows
    xt::xtensor<size_t,1> index = xt::flatten_indices(xt::argwhere(
        m_x >= xt::view(m_pos, xt::all(), m_ntot - m_noff)));

    // allocate new yield distances (times two!)
    xt::xtensor<double,2> y = xt::empty<double>({index.size(), m_ntot});

    // apply buffer: insert previously drawn values
    xt::view(y, xt::all(), xt::range(0, m_nbuf)) =
        xt::view(m_val, xt::keep(index), xt::range(m_ntot - m_nbuf, m_ntot));

    // draw yield distances (times two!)
    xt::view(y, xt::all(), xt::range(m_nbuf, m_ntot)) =
        m_draw({index.size(), m_ntot - m_nbuf});

    // store distances
    xt::view(m_val, xt::keep(index), xt::all()) = y;

    // update positions
    xt::view(y, xt::all(), 0) = xt::view(m_pos, xt::keep(index), m_ntot - m_nbuf);
    xt::view(m_pos, xt::keep(index), xt::all()) = xt::cumsum(y, 1);

    // update current index
    xt::view(m_idx_t, xt::keep(index)) += m_ntot - m_nbuf;
    xt::view(m_idx, xt::keep(index)) -= m_ntot - m_nbuf;

    // register minimum and maximum positions for each particle
    xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
    xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);
}

void RedrawList::extendLeft()
{
    // return if no redraw is needed
    if (!xt::any(m_x <= m_min)) {
        return;
    }

    // get rows for which a redraw is needed
    // use the opportunity to redraw multiple rows
    xt::xtensor<size_t,1> index = xt::flatten_indices(xt::argwhere(
        m_x <= xt::view(m_pos, xt::all(), m_noff)));

    // allocate new yield distances (times two!)
    xt::xtensor<double,2> y = xt::empty<double>({index.size(), m_ntot});

    // apply buffer: insert previously drawn values
    xt::view(y, xt::all(), xt::range(m_ntot - m_nbuf, m_ntot)) =
        xt::view(m_val, xt::keep(index), xt::range(0, m_nbuf));

    // draw yield distances (times two!)
    xt::view(y, xt::all(), xt::range(0, m_ntot - m_nbuf)) =
        m_draw({index.size(), m_ntot - m_nbuf});

    // store distances
    xt::view(m_val, xt::keep(index), xt::all()) = y;

    // update positions
    xt::view(y, xt::all(), 0) = xt::view(m_pos, xt::keep(index), m_nbuf) - xt::sum(y, 1);
    xt::view(m_pos, xt::keep(index), xt::all()) = xt::cumsum(y, 1);

    // update current index
    xt::view(m_idx_t, xt::keep(index)) -= m_ntot - m_nbuf;
    xt::view(m_idx, xt::keep(index)) += m_ntot - m_nbuf;

    // register minimum and maximum positions for each particle
    xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
    xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);
}

xt::xtensor<double,1> RedrawList::currentYieldLeft() const
{
    return m_left;
}

xt::xtensor<double,1> RedrawList::currentYieldRight() const
{
    return m_right;
}

xt::xtensor<long,1> RedrawList::currentIndex() const
{
    return m_idx_t + xt::cast<long>(m_idx);
}

xt::xtensor<double,1> RedrawList::raw_x() const
{
    return m_x;
}

xt::xtensor<double,2> RedrawList::raw_val() const
{
    return m_val;
}

xt::xtensor<double,2> RedrawList::raw_pos() const
{
    return m_pos;
}

xt::xtensor<size_t,1> RedrawList::raw_idx() const
{
    return m_idx;
}

xt::xtensor<long,1> RedrawList::raw_idx_t() const
{
    return m_idx_t;
}

} // namespace QPot

#endif
