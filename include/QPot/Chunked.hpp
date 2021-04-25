/**
\todo Describe

\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_CHUNKED_HPP
#define QPOT_CHUNKED_HPP

#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>
#include <xtensor/xsort.hpp>

#include <xtensor/xio.hpp> // REMOVE

#include "config.h"
#include "version.hpp"

/**
\todo Describe
*/
namespace QPot {

/**
\brief Class supporting the use (frozen) sequence of yield positions supplied in chunks.

\section intro Introduction

Class supporting the use (frozen) sequence of yield positions supplied in chunks around the
current position (to limit storage, speed-up search, or even to avoid defining the full sequence).
Consider a sequence

    Y0  Y1  Y2  Y3  Y4  Y5  Y6  Y7  Y8  Y9  Y10 Y11 Y12 Y13 Y14 ...

The idea is to supply this sequence in chunks, e.g.:

    Y0  Y1  Y2  Y3  Y4                                              if Y0  <= x <= Y4
                        Y5  Y6  Y7  Y8  Y9                          if Y5  <= x <= Y9
                                            Y10 Y11 Y12 Y13 Y14     if Y10 <= x <= Y14
                                                                    ...

Whereby of more practical relevance is to 'buffer' some overlap, e.g.:

    Y0  Y1  Y2  Y3  Y4                                              if Y0  <= x <= Y3
               (Y3  Y4) Y5  Y6  Y7  Y8  Y9                          if Y3  <= x <= Y8
                                   (Y8  Y9) Y10 Y11 Y12 Y13 Y14     if Y8  <= x <= Y14
                                                                    ...

Note thereby that buffering can be done externally, so in this case one would call:

    chuncked.set_y(    y = { Y0,  Y1,  Y2,  Y3,  Y4}            );
    chuncked.rshift_y( y = { Y5,  Y6,  Y7,  Y8,  Y9}, buffer = 2); // buffers {Y3, Y4}
    chuncked.rshift_y( y = {Y10, Y11, Y12, Y13, Y14}, buffer = 2); // buffers {Y8, Y9}

\section distances Distances & cumulative sum

Instead of the sequence of yield position, one can also specify the distance between the yield
positions, letting Chunked() compute the cumulative sum.
Consider the following definition:

    dY0 = Y0
    dY1 = Y1 - Y0
    dY2 = Y2 - Y1
    dY3 = Y3 - Y2
    ...

Then it is clear that:

    Y1 = Y0 + dY1
    Y2 = Y0 + dY1 + dY2
    Y3 = Y0 + dY1 + dY2 + dY3
    ...

or:

    Y = cumsum(dY)

So if one just wants to use yield distance the above example becomes:

    chuncked.set_dy   ( dy = { dY0,  dY1,  dY2,  dY3,  dY4}            ); // dY0 == Y0
    chuncked.rshift_dy( dy = { dY5,  dY6,  dY7,  dY8,  dY9}, buffer = 2);
    chuncked.rshift_dy( dy = {dY10, dY11, dY12, dY13, dY14}, buffer = 2);

\section left Moving left

Note that moving left in the sequence may not require the same chunks. E.g. to arrive back at the
first chunk of the above example, with buffering, we would need e.g. the following:

    Y0  Y1  Y2  Y3  Y4
               (Y3  Y4) Y5  Y6  Y7  Y8  Y9
                                   (Y8  Y9) Y10 Y11 Y12 Y13 Y14 Y15
                Y3  Y4  Y5  Y6  Y7 (Y8  Y9)
    Y0  Y1  Y2 (Y3  Y4)

or in code:

    chuncked.set_y   ( y = { Y0,  Y1,  Y2,  Y3,  Y4}            );
    chuncked.rshift_y( y = { Y5,  Y6,  Y7,  Y8,  Y9}, buffer = 2); // buffers {Y3, Y4}
    chuncked.rshift_y( y = {Y10, Y11, Y12, Y13, Y14}, buffer = 2); // buffers {Y8, Y9}
    chuncked.lshift_y( y = { Y3,  Y4,  Y5,  Y6,  Y7}, buffer = 2); // buffers {Y8, Y9}
    chuncked.lshift_y( y = { Y0,  Y1,  Y2}          , buffer = 2); // buffers {Y3, Y4}

Which again looks a bit different in terms of yield distance:

    chuncked.set_dy   ( dy = { dY0,  dY1,  dY2,  dY3,  dY4}            ); // dY0 == Y0
    chuncked.rshift_dy( dy = { dY5,  dY6,  dY7,  dY8,  dY9}, buffer = 2);
    chuncked.rshift_dy( dy = {dY10, dY11, dY12, dY13, dY14}, buffer = 2);
    chuncked.lshift_dy( dy = { dY4,  dY5,  dY6,  dY7,  dY8}, buffer = 2); // Y7 = Y8 - dY8, ...
    chuncked.lshift_dy( dy = { dY1,  dY2,  dY3}            , buffer = 2); // ..., Y0 = Y1 - dY1

so the same number of arguments this time, but slightly different indexes.

\section index With indexes

To make things more transparent one can provide information about which chunk of yield positions
or yield distances is supplying.
In that case you can supply too much yield positions or yield distances and let Chunked()
ignore items where needed.

In this case the above could be for example a series of the following chucks:

    Y0  Y1  Y2  Y3  Y4
               (Y3  Y4) Y5  Y6  Y7  Y8  Y9
                                   (Y8  Y9) Y10 Y11 Y12 Y13 Y14 Y15
                        Y5  Y6  Y7 (Y8  Y9)
    Y0  Y1  Y2  Y3  Y4 (Y5  Y6)

which, using yield positions, corresponds to the following code

    chuncked.set_y   ( istart = 0 , y = { Y0,  Y1,  Y2,  Y3,  Y4}            );
    chuncked.rshift_y( istart = 5 , y = { Y5,  Y6,  Y7,  Y8,  Y9}, buffer = 2);
    chuncked.rshift_y( istart = 10, y = {Y10, Y11, Y12, Y13, Y14}, buffer = 2);
    chuncked.lshift_y( istart = 5 , y = { Y5,  Y6,  Y7,  Y8,  Y9}, buffer = 2); // {Y8, Y9} 'ignored'
    chuncked.lshift_y( istart = 0 , y = { Y0,  Y1,  Y2,  Y3,  Y4}, buffer = 2);

or using yield distances:

    chuncked.set_dy   ( istart = 0 , dy = { dY0,  dY1,  dY2,  dY3,  dY4}            );
    chuncked.rshift_dy( istart = 5 , dy = { dY5,  dY6,  dY7,  dY8,  dY9}, buffer = 2);
    chuncked.rshift_dy( istart = 10, dy = {dY10, dY11, dY12, dY13, dY14}, buffer = 2);
    chuncked.lshift_dy( istart = 5 , dy = { dY5,  dY6,  dY7,  dY8,  dY9}, buffer = 2);
    chuncked.lshift_dy( istart = 0 , dy = { dY0,  dY1,  dY2,  dY3,  dY4}, buffer = 2);

\note One onlY has to supply the first index, `istart`, the rest is inferred from the size of the chunk.
\warning Always take care of the definition `Y(i) = Y(i - 1) + dY(i)`.
*/
class Chunked
{
public:

    Chunked() = default;


    /**
    Constructor.

    \param x The current position.
    \param y A chunk of yield-positions.
    \param istart The global index of `y(0)`.
    */
    Chunked(double x, const xt::xtensor<double, 1>& y, long istart = 0)
    {
        m_x = x;
        this->set_y(istart, y);
    };

    /**
    Return the index of the yield-position directly left of the current position.
    \warning This is a global index, which is (generally) not the index in the current chunk.
    */
    long i()
    {
        return m_istart + static_cast<long>(m_li);
    };

    /**
    Customise proximity search region.
    \param distance Width of the region to consider as proximity (default: 10).
    */
    void search_proximity(size_t distance)
    {
        m_proximity = distance;
    };

    /**
    Get the global index of y()(0) = ymin(), the left-most yield position of this chunk.
    \return Index (signed).
    */
    long istart() const
    {
        return m_istart;
    };

    /**
    Get the global index of the slice y() = Y[istart(): istop()].
    \return Index (signed).
    */
    long istop() const
    {
        return m_istop;
    };

    /**
    Get the position of the left-most yield position of the current chunk, ymin() = y()(0).
    \return Yield position.
    */
    double ymin() const
    {
        return m_ymin;
    };

    /**
    Get the position of the right-most yield position of the current chunk, y()(-1) = ymax().
    \return Yield position.
    */
    double ymax() const
    {
        return m_ymax;
    };

    /**
    Get the entire current chunk of yield-positions.
    \return List of yield-positions.
    */
    xt::xtensor<double, 1> y() const
    {
        return m_y;
    };

    /**
    Get yield-positions along a slice.
    \param start Start index of the slice (global index).
    \param stop Stop index of the slice (global index).
    \return List of yield-positions.
    \warning `start` and `stop` must be in the current chunk.
    */
    xt::xtensor<double, 1> y(long start, long stop) const
    {
        QPOT_ASSERT(start <= stop);
        QPOT_ASSERT(start >= m_istart);
        QPOT_ASSERT(stop <= m_istop);

        size_t i = static_cast<size_t>(start - m_istart);
        size_t j = static_cast<size_t>(stop - m_istart);

        return xt::view(m_y, xt::range(i, j));
    };

    /**
    Get a specific yield-position.
    \param i Global index.
    \return Yield position.
    \warning `i` must be in the current chunk.
    */
    double y(long i) const
    {
        QPOT_ASSERT(i >= m_istart);
        QPOT_ASSERT(i < m_istop);

        return m_y(static_cast<size_t>(i - m_istart));
    };

    /**
    Reset the system: Overwrite the current chunk and global index.
    \param istart The global index of `y(0)`.
    \param y A chunk of yield-positions.
    */
    void set_y(long istart, const xt::xtensor<double, 1>& y)
    {
        QPOT_DEBUG(xt::allclose(xt::sort(y), y));

        m_y = y;
        m_n = m_y.size();
        m_istart = istart;
        m_istop = m_istart + static_cast<long>(m_n);
        m_ymin = m_y(0);
        m_ymax = m_y(m_n - 1);
        m_full_search = true;

        this->update();
    };

    /**
    Reset the system: Overwrite the current chunk and global index.
    \param y A chunk of yield-positions.
    \param istart The global index of `y(0)`.
    */
    void set_y(const xt::xtensor<double, 1>& y, long istart = 0)
    {
        this->set_y(istart, y);
    };

//    void set_dy(long istart, const xt::xtensor<double, 1>& dy)
//    {
//        this->set_y(istart, xt::cumsum(dy));
//  };

    /**
    Reset the system: Overwrite the current chunk and global index.
    Specify the yield-distances rather than the yield-positions.
    The yield-positions are constructed as follows:

        y = [dy(0), dy(0) + dy(1), dy(0) + dy(1) + dy(2), ...]

    With the following global index:

        [istart, istart + 1, istart + 2, ...]

    \param dy
        The distances between the yield-positions.
        Note that the first item is interpreted as a yield position.

    \param istart The global index of `dy(0) = y(0)`.
    */
//    void set_dy(const xt::xtensor<double, 1>& dy, long istart = 0)
//    {
//        this->set_y(istart, xt::cumsum(dy));
//    };

    /**
    Right-shift the chunk.
    \param istart The global index of `y(0)`.
    \param y A chunk of yield-positions (following at least partly the current chunk).
    \param nbuffer Buffer the right `nbuffer` yield positions of the current chunk.

    \note If `istart` < istart() - `nbuffer`,
    more is 'buffered' such that the sequence starts at least at `istart`.

    \tip Things are most efficient if the internal memory does not change size. That is the case
    if istop() - istart() is the same before and after this operation.
    */
    void rshift_y(long istart, const xt::xtensor<double, 1>& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        QPOT_ASSERT(istart <= m_istop);
        QPOT_ASSERT(istart >= m_istart); // otherwise the function does not what it's name suggests

        // if there is overlap between the chunks of Y in memory and input:
        // check that the values are indeed the same
        #ifdef QPOT_ENABLE_DEBUG
            if (istart < m_istop) {
                size_t d = static_cast<size_t>(m_istop - istart);
                size_t i = m_n - d;
                xt::xtensor<double, 1> o = xt::view(m_y, xt::range(i, m_n));
                xt::xtensor<double, 1> n = xt::view(y, xt::range(0, d));
                QPOT_DEBUG(xt::allclose(o, n));
            }
        #endif

        // supplied chunk smaller than the chunk in memory
        if (istart >= m_istart && istart + static_cast<long>(y.size()) <= m_istop) {
            return;
        }

        m_istart = m_istop - static_cast<long>(nbuffer);

        // take everything for "y": it spans the new entries and the buffered old entries
        if (istart <= m_istart) {
            return this->set_y(istart, y);
        }

        // buffer "m_y[m_n - nbuffer:]" and take the rest of "y"
        QPOT_ASSERT(nbuffer <= m_n);
        QPOT_DEBUG(xt::allclose(xt::sort(y), y));
        size_t d = static_cast<size_t>(m_istop - istart); // items to skip from "y"
        m_istop = istart + static_cast<long>(y.size());
        size_t n = static_cast<size_t>(m_istop - m_istart);

        if (n != m_n) {
            xt::xtensor<double, 1> t = xt::view(m_y, xt::range(m_n - nbuffer, m_n));
            m_n = n;
            m_y.resize({m_n});
            std::copy(t.cbegin(), t.cend(), m_y.begin());
        }
        else {
            std::copy(m_y.cend() - nbuffer, m_y.cend(), m_y.begin());
        }

        std::copy(y.cbegin() + d, y.cend(), m_y.begin() + nbuffer);

        m_ymin = m_y(0);
        m_ymax = m_y(m_n - 1);
        m_full_search = true;

        this->update();
    };

    /**
    Right-shift the chunk.
    \param y A chunk of yield-positions (preceding the current chunk).
    \param nbuffer Buffer the right `nbuffer` yield positions of the current chunk.
    */
    void rshift_y(const xt::xtensor<double, 1>& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        this->rshift_y(m_istop, y, nbuffer);
    };

    /**
    Right-shift the chunk using yield-distances (rather than yield-positions).
    \param dy The yield-distances following the current right-most yield position.
    \param nbuffer Buffer a part of the current chunk.
    */
    void rshift_dy(const xt::xtensor<double, 1>& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);
        this->rshift_y(m_istop, m_y(m_n - 1) + xt::cumsum(dy), nbuffer);
    };

    void rshift_dy(long istart, const xt::xtensor<double, 1>& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);
        QPOT_ASSERT(istart >= m_istart && istart <= m_istop);
        QPOT_ASSERT(istart + static_cast<long>(dy.size()) >= m_istop);

        auto y = xt::cumsum(dy);
        y += m_y(istart - m_istart) - y(0);

        this->rshift_y(istart, y, nbuffer);
    };


    void lshift_y(long istart, const xt::xtensor<double, 1>& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        long istop = istart + static_cast<long>(y.size());
        QPOT_ASSERT(istart <= m_istart);
        QPOT_ASSERT(istop >= m_istart);

        #ifdef QPOT_ENABLE_DEBUG
            if (istop > m_istart) {
                size_t d = static_cast<size_t>(istop - m_istart);
                xt::xtensor<double, 1> o = xt::view(m_y, xt::range(0, d));
                xt::xtensor<double, 1> n = xt::view(y, xt::range(y.size() - d, y.size()));
                QPOT_DEBUG(xt::allclose(o, n));
            }
        #endif

        // supplied chunk smaller than the chunk in memory
        if (istart >= m_istart && istart + static_cast<long>(y.size()) <= m_istop) {
            return;
        }

        m_istop = m_istart + nbuffer;

        // take everything for "y": it spans the new entries and the buffered old entries
        if (istop >= m_istop) {
            return this->set_y(istart, y);
        }

        // buffer "m_y[: nbuffer]" and take the rest of "y"
        QPOT_ASSERT(nbuffer <= m_n);
        QPOT_DEBUG(xt::allclose(xt::sort(y), y));
        size_t d = static_cast<size_t>(istop - m_istart); // items to skip from "y"
        m_istart = istart;
        size_t n = static_cast<size_t>(m_istop - m_istart);

        if (n != m_n) {
            xt::xtensor<double, 1> t = xt::view(m_y, xt::range(0, nbuffer));
            m_n = n;
            m_y.resize({m_n});
            std::copy(t.cbegin(), t.cend(), m_y.end() - nbuffer);
        }
        else {
            std::copy(m_y.cbegin(), m_y.cbegin() + nbuffer, m_y.end() - nbuffer);
        }

        std::copy(y.cbegin(), y.cend() - d, m_y.begin());

        m_ymin = m_y(0);
        m_ymax = m_y(m_n - 1);
        m_full_search = true;

        this->update();
    };

    void lshift_y(const xt::xtensor<double, 1>& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        return this->lshift_y(m_istart - y.size(), y, nbuffer);
    };

    void lshift_dy(const xt::xtensor<double, 1>& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);
        auto y = xt::cumsum(dy);
        y -= y(y.size() - 1) - m_y(0);
        this->lshift_y(m_istart - static_cast<long>(y.size()) + 1, y, nbuffer);
    };

    void lshift_dy(long istart, const xt::xtensor<double, 1>& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);
        QPOT_ASSERT(istart + static_cast<long>(dy.size()) > m_istart);
        auto y = xt::cumsum(dy);
        y -= y(y.size() - 1) - m_y(istart + static_cast<long>(dy.size()) - m_istart - 1);
        this->lshift_y(istart, y, nbuffer);
    };

    /**
    Set the current position.
    \param x The current position.
    */
    void set_x(double x)
    {
        m_x = x;
        this->update();
    };

    /**
    Check for redraw.
    */
    int redraw() const
    {
        return m_redraw;
    }

    double yi_left() const
    {
        QPOT_ASSERT(!m_lock);
        return m_left;
    }

    double yi_right() const
    {
        QPOT_ASSERT(!m_lock);
        return m_right;
    }

    double yi_left(size_t offset) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(offset <= m_li);
        return m_y(m_li - offset);
    }

    double yi_right(size_t offset) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(m_li + 1 + offset < m_n);
        return m_y(m_li + 1 + offset);
    }

    double yi(int offset) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(offset != 0);

        if (offset < 0) {
            size_t shift = static_cast<size_t>(- offset);
            QPOT_ASSERT(shift - 1 <= m_li);
            return m_y(m_li - shift + 1);
        }

        size_t shift = static_cast<size_t>(offset);
        QPOT_ASSERT(m_li + shift < m_n);
        return m_y(m_li + shift);
    }

    bool boundcheck_left(size_t n = 0) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(n < m_n);
        return m_li > n;
    }

    bool boundcheck_right(size_t n = 0) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(n < m_n);
        return m_li < m_n - n;
    }

private:

    void update()
    {
        if (m_x <= m_ymin) {
            m_redraw = -1;
        }
        else if (m_x > m_ymax) {
            m_redraw = +1;
        }
        else {
            m_redraw = 0;
        }

        m_lock = m_redraw != 0;

        if (m_lock) {
            return;
        }

        if (m_full_search) {
            m_li = std::lower_bound(&m_y(0), &m_y(0) + m_n, m_x) - &m_y(0) - 1;
            m_left = m_y(m_li);
            m_right = m_y(m_li + 1);
            m_full_search = false;
            return;
        }

        if (m_left < m_x && m_right >= m_x) {
            return;
        }

        size_t l = m_li > m_proximity ? m_li - m_proximity : 0;
        size_t r = std::min(m_li + m_proximity, m_n - 1);

        if (m_y(l) < m_x && m_y(r) >= m_x) {
            m_li = std::lower_bound(&m_y(l), &m_y(l) + r - l, m_x) - &m_y(l) - 1 + l;
        }
        else {
            m_li = std::lower_bound(&m_y(0), &m_y(0) + m_n, m_x) - &m_y(0) - 1;
        }

        m_left = m_y(m_li);
        m_right = m_y(m_li + 1);
    }

private:

    size_t m_li;
    size_t m_n;
    size_t m_proximity = 10;
    long m_istart;
    long m_istop;
    double m_ymin;
    double m_ymax;
    double m_x = 0.0;
    int m_redraw;
    bool m_lock = true;
    bool m_full_search = true;
    double m_left;
    double m_right;
    xt::xtensor<double, 1> m_y;

}; // class Chunked

} // namespace QPot

#endif






