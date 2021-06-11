/**
Dynamically redraw yielding positions.

\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_REDRAW_HPP
#define QPOT_REDRAW_HPP

#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xnoalias.hpp>

#include "config.h"
#include "version.hpp"

namespace QPot {

/**
Yielding positions (that are dynamically redrawn) for a list of particles.
*/
class RedrawList
{
public:

    RedrawList() = default;

    /**
    Constructor.

    \tparam E
        E.g. ``xt::xtensor<double, 1>``.

    \tparam F
        E.g. ``std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)>``.

    \param x
        Current positions, number of particles derived from it.

    \param func
        Function to draw yield distances.

    \param ntotal
        Number of yield-positions to keep in memory.

    \param nbuffer
        Number of yield-positions to buffer when shifting left/right.

    \param noffset
        Number of yield-positions from the end to consider for redrawing
        (allows grouping of redraws for several particles).
    */
    template <class E, class F>
    RedrawList(const E& x, F func, size_t ntotal = 1000, size_t nbuffer = 300, size_t noffset = 20);

    /**
    Number of particles.

    \return unsigned int
    */
    size_t N() const;

    /**
    Yield positions.

    \return All yield positions [#N, ntotal].
    */
    xt::xtensor<double, 2> yieldPosition() const; // y

    /**
    Slice yieldPosition()[:, start: stop].

    \param start Lower column bound.
    \param stop Upper column bound.
    \return Yield positions [#N, stop - start].
    */
    xt::xtensor<double, 2> yieldPosition(size_t start, size_t stop) const;

    /**
    Slice yieldPosition()[:, i].

    \param i Column to select.
    \return Yield positions [#N].
    */
    xt::xtensor<double, 1> yieldPosition(size_t i) const;

    /**
    Customise proximity search region (default: ``10``).

    \param distance Width of the region to consider as proximity.
    */
    void setProximity(size_t distance);

    /**
    Update current positions.

    \tparam E E.g. ``xt::xtensor<double, 1>``.
    \param x Current positions [#N].
    \return ``true`` is the was a redraw. See currentRedraw() for details on the redraw.
    */
    template <class E>
    bool setPosition(const E& x);

    /**
    Yielding positions at an offset left/right of the current position:
    -   offset > 0: ``yieldPosition()[:, current_index + offset]``
    -   offset < 0: ``yieldPosition()[:, current_index + offset + 1]``

    \param offset The offset (same for all particles).
    \return Yield positions [#N].
    */
    xt::xtensor<double, 1> nextYield(int offset) const;

    /**
    Yielding positions left of the current position

        yieldPosition()[:, current_index]

    \return Yield positions [#N].
    */
    xt::xtensor<double, 1> currentYieldLeft() const;

    /**
    Yielding positions right of the current position

        yieldPosition()[:, current_index + 1]

    \return Yield positions [#N].
    */
    xt::xtensor<double, 1> currentYieldRight() const;

    /**
    Yielding positions at an offset left of the current position

        yieldPosition()[:, current_index - offset]

    \param offset The offset (same for all particles).
    \return Yield positions [#N].
    */
    xt::xtensor<double, 1> currentYieldLeft(size_t offset) const;

    /**
    Yielding positions at an offset right of the current position

        yieldPosition()[:, current_index + 1 + offset]

    \param offset The offset (same for all particles).
    \return Yield positions [#N].
    */
    xt::xtensor<double, 1> currentYieldRight(size_t offset) const;

    /**
    Slice of yielding positions at an offset left/right of the current position

        yieldPosition()[:, current_index + left: current_index + right]

    \param left Offset left (same for all particles).
    \param right Offset right (same for all particles).
    \return Yield positions [#N, right - left].
    */
    xt::xtensor<double, 2> currentYield(int left, int right) const;

    /**
    Index of the current minima

        yieldPosition()[:, index] -> yielding positions left
        yieldPosition()[:, index + 1] -> yielding positions right

    \return Indices [#N].
    */
    xt::xtensor<long, 1> currentIndex() const;

    /**
    Check if there was a redraw the last time setPosition() was called.
    See currentRedraw() for details on the redraw.

    \return bool
    */
    bool currentAnyRedraw() const;

    /**
    Get the direction of redrawing the last time setPosition() was called.

    \return Direction (+1: redraw right, -1 redraw left, 0 no redraw) [#N].
    */
    xt::xtensor<int, 1> currentRedraw() const;

    /**
    List of particles that were redrawn right, the last time setPosition() was called.

    \return List of particles.
    */
    xt::xtensor<size_t, 1> currentRedrawRight() const;

    /**
    List of particles that were redrawn right, the last time setPosition() was called.

    \return List of particles.
    */
    xt::xtensor<size_t, 1> currentRedrawLeft() const;

    /**
    Force redraw (can be used to restore a sequence).

    \tparam T E.g. ``xt::xtensor<int, 1>``.
    \param iredraw See currentRedraw().
    */
    template <class T>
    void redraw(const T& iredraw);

    /**
    Force redraw (can be used to restore a sequence).

    \tparam T E.g. ``xt::xtensor<size_t, 1>``.
    \param index List of particles for which to redraw to the right.
    */
    template <class T>
    void redrawRight(const T& index);

    /**
    Force redraw (can be used to restore a sequence).

    \tparam T E.g. ``xt::xtensor<size_t, 1>``.
    \param index List of particles for which to redraw to the left.
    */
    template <class T>
    void redrawLeft(const T& index);

    /**
    Output the current matrix of random values.

    \return [#N, ntotal].
    */
    xt::xtensor<double, 2> raw_val() const;

    /**
    Output the current matrix of yielding positions.

    \return [#N, ntotal].
    */
    xt::xtensor<double, 2> raw_pos() const;

    /**
    Output the current matrix of indices.

    \return [#N].
    */
    xt::xtensor<size_t, 1> raw_idx() const;

    /**
    Output the current matrix of historic indices.

    \return [#N].
    */
    xt::xtensor<long, 1> raw_idx_t() const;

    /**
    Restore raw data.

    \param val See raw_val().
    \param pos See raw_pos().
    \param idx See raw_idx().
    \param idx_t See raw_idx_t().
    */
    void restore(
        const xt::xtensor<double, 2>& val,
        const xt::xtensor<double, 2>& pos,
        const xt::xtensor<size_t, 1>& idx,
        const xt::xtensor<long, 1>& idx_t);

private:

    /**
    Compute the current #m_idx for all particles.
    Internally the following rules are used:

    1.  If a particle's yield positions were redrawn: global search.
    2.  See if the index is unchanged.
    3.  Search the near surroundings.
    4.  Global search.
    */
    template <class E>
    void computeIndex(const E& x);

private:

    size_t m_N; ///< Number of points
    size_t m_ntot; ///< Number of yield positions to keep in memory at all times.
    size_t m_nbuf; ///< Number of yield positions to buffer.
    size_t m_noff; ///< Number of yield positions at which to shift maximally.
    size_t m_proximity = 10; ///< Size of neighbourhood to search first.
    xt::xtensor<int, 1> m_iredraw; ///< See RedrawList::iredraw
    bool m_redraw = false; ///< `See RedrawList::redraw

    /**
    Lock outputting state:
    If redraw(), redrawRight(), redrawLeft() are externally called,
    the current position is unknown.
    */
    bool m_lock = false;

    /**
    Yielding positions

        = cumsum(m_val, axis=1) + init

    (``init`` is computed during initialisation and redraw).
    */
    xt::xtensor<double, 2> m_pos;

    xt::xtensor<double, 2> m_val; ///< Drawn random values.
    xt::xtensor<size_t, 1> m_idx; ///< Current "index": since the last shift.
    xt::xtensor<long, 1> m_idx_t; ///< Current "index": total up to the last shift (can be negative).
    xt::xtensor<double, 1> m_max; ///< Maximum yielding positions.
    xt::xtensor<double, 1> m_min; ///< Minimum yielding positions.
    xt::xtensor<double, 1> m_left; ///< Current yielding positions to the left ``== m_pos[:, m_idx]``.
    xt::xtensor<double, 1> m_right; ///< Current yielding positions to the right ``== m_pos[:, m_idx + 1]``.

    /**
    Function to (re)draw yield positions.
    */
    std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)> m_draw;
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
    QPOT_ASSERT(2 * m_noff < m_ntot); // either redraw left or right, never both for the same "x"

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
    m_iredraw = xt::zeros<decltype(m_iredraw)::value_type>({m_N});
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

inline size_t RedrawList::N() const
{
    return m_N;
}

inline void RedrawList::setProximity(size_t proximity)
{
    m_proximity = proximity;
}

inline xt::xtensor<double, 2> RedrawList::yieldPosition() const
{
    return m_pos;
}

inline xt::xtensor<double, 2> RedrawList::yieldPosition(size_t start, size_t stop) const
{
    return xt::view(m_pos, xt::all(), xt::range(start, stop));
}

inline xt::xtensor<double, 1> RedrawList::yieldPosition(size_t i) const
{
    QPOT_ASSERT(i < m_ntot);
    return xt::view(m_pos, xt::all(), i);
}

inline xt::xtensor<double, 1> RedrawList::currentYieldLeft(size_t offset) const
{
    QPOT_ASSERT(!m_lock);
    QPOT_ASSERT(offset <= xt::amin(m_idx)());
    xt::xtensor<double, 1> ret = xt::empty<double>({m_N});
    for (size_t p = 0; p < m_N; ++p) {
        ret(p) = m_pos(p, m_idx(p) - offset);
    }
    return ret;
}

inline xt::xtensor<double, 1> RedrawList::currentYieldRight(size_t offset) const
{
    QPOT_ASSERT(!m_lock);
    QPOT_ASSERT(xt::amax(m_idx)() + 1 + offset < m_ntot);
    xt::xtensor<double, 1> ret = xt::empty<double>({m_N});
    for (size_t p = 0; p < m_N; ++p) {
        ret(p) = m_pos(p, m_idx(p) + 1 + offset);
    }
    return ret;
}

inline xt::xtensor<double, 1> RedrawList::nextYield(int offset) const
{
    QPOT_ASSERT(!m_lock);
    QPOT_ASSERT(offset != 0);

    if (offset < 0) {
        size_t shift = static_cast<size_t>(- offset);
        QPOT_ASSERT(shift - 1 <= xt::amin(m_idx)());
        xt::xtensor<double, 1> ret = xt::empty<double>({m_N});
        for (size_t p = 0; p < m_N; ++p) {
            ret(p) = m_pos(p, m_idx(p) - shift + 1);
        }
        return ret;
    }

    size_t shift = static_cast<size_t>(offset);
    QPOT_ASSERT(xt::amax(m_idx)() + shift < m_ntot);
    xt::xtensor<double, 1> ret = xt::empty<double>({m_N});
    for (size_t p = 0; p < m_N; ++p) {
        ret(p) = m_pos(p, m_idx(p) + shift);
    }
    return ret;
}

inline xt::xtensor<double, 2> RedrawList::currentYield(int left, int right) const
{
    QPOT_ASSERT(!m_lock);
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

inline xt::xtensor<double, 1> RedrawList::currentYieldLeft() const
{
    QPOT_ASSERT(!m_lock);
    return m_left;
}

inline xt::xtensor<double, 1> RedrawList::currentYieldRight() const
{
    QPOT_ASSERT(!m_lock);
    return m_right;
}

inline xt::xtensor<long, 1> RedrawList::currentIndex() const
{
    QPOT_ASSERT(!m_lock);
    return m_idx_t + xt::cast<long>(m_idx);
}

template <class T>
inline void RedrawList::redraw(const T& iredraw)
{
    QPOT_ASSERT(iredraw.size() == m_N);
    QPOT_ASSERT(iredraw.dimension() == 1);
    this->redrawRight(xt::flatten_indices(xt::argwhere(iredraw > 0)));
    this->redrawLeft(xt::flatten_indices(xt::argwhere(iredraw < 0)));
}

template <class T>
inline void RedrawList::redrawRight(const T& index)
{
    // store redraw
    m_lock = true;
    m_redraw = true;
    xt::view(m_iredraw, xt::keep(index)) = +1;

    // allocate new yield distances (times two!)
    xt::xtensor<double, 2> y = xt::empty<double>({index.size(), m_ntot});

    // apply buffer: insert previously drawn values
    xt::view(y, xt::all(), xt::range(0, m_nbuf)) =
        xt::view(m_val, xt::keep(index), xt::range(m_ntot - m_nbuf, m_ntot));

    // draw yield distances (times two!)
    xt::view(y, xt::all(), xt::range(m_nbuf, m_ntot)) = m_draw({index.size(), m_ntot - m_nbuf});
    QPOT_ASSERT(xt::all(y > 0));

    // store yield distances
    xt::view(m_val, xt::keep(index), xt::all()) = y;

    // update yield positions
    xt::view(y, xt::all(), 0) = xt::view(m_pos, xt::keep(index), m_ntot - m_nbuf);
    xt::view(m_pos, xt::keep(index), xt::all()) = xt::cumsum(y, 1);

    // register minimum and maximum yield positions for each particle
    xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
    xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);

    // update historic/current index
    xt::view(m_idx_t, xt::keep(index)) += m_ntot - m_nbuf;
}

template <class T>
inline void RedrawList::redrawLeft(const T& index)
{
    // store redraw
    m_lock = true;
    m_redraw = true;
    xt::view(m_iredraw, xt::keep(index)) = -1;

    // allocate new yield distances (times two!)
    xt::xtensor<double, 2> y = xt::empty<double>({index.size(), m_ntot});

    // apply buffer: insert previously drawn values
    xt::view(y, xt::all(), xt::range(m_ntot - m_nbuf, m_ntot)) =
        xt::view(m_val, xt::keep(index), xt::range(0, m_nbuf));

    // draw yield distances (times two!)
    xt::view(y, xt::all(), xt::range(0, m_ntot - m_nbuf)) = m_draw({index.size(), m_ntot - m_nbuf});
    QPOT_ASSERT(xt::all(y > 0));

    // store yield distances
    xt::view(m_val, xt::keep(index), xt::all()) = y;

    // update yield positions
    xt::view(y, xt::all(), 0) = xt::view(m_pos, xt::keep(index), m_nbuf) - xt::sum(y, 1);
    xt::view(m_pos, xt::keep(index), xt::all()) = xt::cumsum(y, 1);

    // register minimum and maximum yield positions for each particle
    xt::noalias(m_min) = xt::view(m_pos, xt::all(), 0);
    xt::noalias(m_max) = xt::view(m_pos, xt::all(), m_pos.shape(1) - 1);

    // update historic/current index
    xt::view(m_idx_t, xt::keep(index)) -= m_ntot - m_nbuf;
}

template <class E>
inline void RedrawList::computeIndex(const E& x)
{
    using index_type = decltype(m_idx)::value_type;
    using position_type = typename E::value_type;

    if (!m_redraw) {
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
                i = std::lower_bound(&m_pos(p,0), &m_pos(p,0) + m_ntot, xp) - &m_pos(p,0) - 1;
            }

            m_idx(p) = i;
            m_left(p) = m_pos(p, i);
            m_right(p) = m_pos(p, i + 1);
        }
    }
    else {
        for (size_t p = 0; p < m_N; ++p)
        {
            index_type i = m_idx(p);
            position_type xp = x(p);
            bool redraw = m_iredraw(p) != 0;

            if (m_left(p) < xp && m_right(p) >= xp && !redraw) {
                continue;
            }

            index_type l = i > m_proximity ? i - m_proximity : 0;
            index_type r = std::min(i + m_proximity, m_ntot - 1);

            if (m_pos(p, l) < xp && m_pos(p, r) >= xp && !redraw) {
                i = std::lower_bound(&m_pos(p,l), &m_pos(p,l) + r - l, xp) - &m_pos(p,l) - 1 + l;
            }
            else {
                i = std::lower_bound(&m_pos(p,0), &m_pos(p,0) + m_ntot, xp) - &m_pos(p,0) - 1;
            }

            m_idx(p) = i;
            m_left(p) = m_pos(p, i);
            m_right(p) = m_pos(p, i + 1);
        }
    }

    m_lock = false;
}

inline bool RedrawList::currentAnyRedraw() const
{
    QPOT_ASSERT(!m_lock);
    return m_redraw;
}

inline xt::xtensor<int, 1> RedrawList::currentRedraw() const
{
    QPOT_ASSERT(!m_lock);
    return m_iredraw;
}

inline xt::xtensor<size_t, 1> RedrawList::currentRedrawRight() const
{
    QPOT_ASSERT(!m_lock);
    return xt::flatten_indices(xt::argwhere(m_iredraw > 0));
}

inline xt::xtensor<size_t, 1> RedrawList::currentRedrawLeft() const
{
    QPOT_ASSERT(!m_lock);
    return xt::flatten_indices(xt::argwhere(m_iredraw < 0));
}

template <class E>
inline bool RedrawList::setPosition(const E& x)
{
    QPOT_ASSERT(x.size() == m_N);

    // if redraw was force-triggered (from redrawRight or redrawLeft and not from here):
    // make sure that all indices are updated (forces update for 'redrawn particles')
    if (m_lock) {
        this->computeIndex(x);
    }

    // erase history
    if (m_redraw) {
        m_iredraw.fill(0);
        m_redraw = false;
    }

    // extend right (use the opportunity to redraw multiple rows)
    if (xt::any(x >= m_max))
    {
        xt::xtensor<size_t, 1> index = xt::flatten_indices(xt::argwhere(
            x >= xt::view(m_pos, xt::all(), m_ntot - m_noff)));

        this->redrawRight(index);
    }

    // extend left (use the opportunity to redraw multiple rows)
    if (xt::any(x <= m_min))
    {
        // get rows for which a redraw is needed
        // use the opportunity to redraw multiple rows
        xt::xtensor<size_t, 1> index = xt::flatten_indices(xt::argwhere(
            x <= xt::view(m_pos, xt::all(), m_noff)));

        this->redrawLeft(index);
    }

    // update index (force update 'redrawn particles')
    this->computeIndex(x);

    return m_redraw;
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
