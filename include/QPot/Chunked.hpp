/**
Class supporting the use (frozen) sequence of yield positions supplied in chunks.

\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#ifndef QPOT_CHUNKED_HPP
#define QPOT_CHUNKED_HPP

#include <vector>
#include <numeric>

#ifdef QPOT_ENABLE_DEBUG
#include <xtensor/xtensor.hpp>
#endif

#include "config.h"
#include "version.hpp"

/**
Store sequence of yield positions (in chunks) and use it to find the yield positions
left and right of a specific position.
*/
namespace QPot {

/**
\brief Class supporting the use (frozen) sequence of yield positions supplied in chunks.

\section intro Introduction

Class supporting the use (frozen) sequence of yield positions supplied in chunks around the
current position (to limit storage, speed-up search, or even to avoid defining the full sequence).
Consider a sequence

    Y0  Y1  Y2  Y3  Y4  Y5  Y6  Y7  Y8  Y9  Y10 Y11 Y12 Y13 Y14 ...

whereby the capital `Y` refers to the 'global' sequence and the small `y` to a chunk.

The idea is to supply this sequence in chunks, e.g.:

    Y0  Y1  Y2  Y3  Y4                                              if Y0  <= x <= Y4
                        Y5  Y6  Y7  Y8  Y9                          if Y5  <= x <= Y9
                                            Y10 Y11 Y12 Y13 Y14     if Y10 <= x <= Y14
                                                                    ...

Which look as follows in (pseudo-)code:

    chuncked.set_y(    y = { Y0,  Y1,  Y2,  Y3,  Y4});
    chuncked.rshift_y( y = { Y5,  Y6,  Y7,  Y8,  Y9});
    chuncked.rshift_y( y = {Y10, Y11, Y12, Y13, Y14});

or even more schematically:

    chuncked.set_y(    y = Y[ 0:  5]);
    chuncked.rshift_y( y = Y[ 5: 10]);
    chuncked.rshift_y( y = Y[10: 15]);

\note A word of attention, without specifying any global index concerning the new chunk,
`rshift_y` and `rshift_dy` (or `lshift_y` and `lshift_dy`), will infer the index by assuming that the
supplied data is exactly the next chunk on the right (or left).
If one does not pay attention the can lead to unwanted results.
Fortunately, Chunked allows you to specify the global index of the first item of the new chunk.
This will enable assertions, and allow you to be loose about what data the specify:
As long as the needed data is part of the supplied chunk, Chunked will know what to do.

\section buffer Buffer

It is of practical relevance to 'buffer' some overlap, e.g.:

    Y0  Y1  Y2  Y3  Y4                                              if Y0  <= x <= Y3
               (Y3  Y4) Y5  Y6  Y7  Y8  Y9                          if Y3  <= x <= Y9
                                   (Y8  Y9) Y10 Y11 Y12 Y13 Y14     if Y8  <= x <= Y14
                                                                    ...

Which, in pseudo-code, is implemented simply as:

    chuncked.set_y(    y = Y[ 0:  5]            );
    chuncked.rshift_y( y = Y[ 5: 10], buffer = 2);
    chuncked.rshift_y( y = Y[10: 15], buffer = 2);

\section distances Distances & cumulative sum

Instead of the sequence of yield position, one can also specify the distance between the yield
positions, letting Chunked compute the cumulative sum.
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

So, if one just wants to use yield distance the above example becomes:

    chuncked.set_y    ( y  =  Y[ 0:  5]            );
    chuncked.rshift_dy( dy = dY[ 5: 10], buffer = 2);
    chuncked.rshift_dy( dy = dY[10: 15], buffer = 2);

\section left Moving left

To arrive back at the first chunk of the above example, everything is straightforward without
buffering. For example:

    Y0  Y1  Y2  Y3  Y4
                        Y5  Y6  Y7  Y8  Y9
                                            Y10 Y11 Y12 Y13 Y14 Y15
                        Y5  Y6  Y7  Y8  Y9
    Y0  Y1  Y2  Y3  Y4

Or in code:

    chuncked.set_y(    y = Y[ 0:  5]);
    chuncked.rshift_y( y = Y[ 5: 10]);
    chuncked.rshift_y( y = Y[10: 15]);
    chuncked.lshift_y( y = Y[ 5: 10]);
    chuncked.lshift_y( y = Y[ 0:  5]);

With buffering, however, we should be careful what to pass.
Consider the same example with a buffer of two:

    Y0  Y1  Y2  Y3  Y4
               (Y3  Y4) Y5  Y6  Y7  Y8  Y9
                                   (Y8  Y9) Y10 Y11 Y12 Y13 Y14 Y15
                        Y5  Y6  Y7 (Y8  Y9)
    Y0  Y1  Y2  Y3  Y4 (Y5  Y6)

or in code:

    chuncked.set_y   ( y = Y[ 0:  5]            );
    chuncked.rshift_y( y = Y[ 5: 10], buffer = 2);
    chuncked.rshift_y( y = Y[10: 15], buffer = 2);
    chuncked.lshift_y( y = Y[ 5:  8], buffer = 2);
    chuncked.lshift_y( y = Y[ 0:  5], buffer = 2);

This can be simplified by also supplying the starting index of the supplied chunk
(such that it does not need to be inferred,
and new chunks only have to encompass what Chunked needs):

    chuncked.set_y   ( istart =  0, y = Y[ 0:  5]            );
    chuncked.rshift_y( istart =  5, y = Y[ 5: 10], buffer = 2);
    chuncked.rshift_y( istart = 10, y = Y[10: 15], buffer = 2);
    chuncked.lshift_y( istart =  5, y = Y[ 5: 10], buffer = 2);
    chuncked.lshift_y( istart =  0, y = Y[ 0:  5], buffer = 2);

The latter strategy makes using yield distances transparent:

    chuncked.set_y   ( istart =  0, y = Y[ 0:  5]            );
    chuncked.rshift_y( istart =  5, y = Y[ 5: 10], buffer = 2);
    chuncked.rshift_y( istart = 10, y = Y[10: 15], buffer = 2);
    chuncked.lshift_y( istart =  5, y = Y[ 5: 10], buffer = 2);
    chuncked.lshift_y( istart =  0, y = Y[ 0:  6], buffer = 2);

\warning Be aware of the definition `Y(i) = Y(i - 1) + dY(i)`. This has the consequence that
the slice on the last line stops at 6 (an not at 5 as when specifying yield positions),
because Y5 = Y4 + dY5 and thus that Y4 = Y4 - dY5.

\note One only has to supply the first index, `istart`,
the rest is inferred from the size of the chunk.
*/
class Chunked
{
public:

    Chunked() = default;


    /**
    Constructor.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param x The current position.
    \param y A chunk of yield positions.
    \param istart The global index of `y[0]`.
    */
    template <class T>
    Chunked(double x, const T& y, long istart = 0)
    {
        m_x = x;
        this->set_y(istart, y);
    }

    /**
    Return the global index of the yield-position directly left of x().
    \return Index (signed).
    \warning The global index is generally not the index in the current chunk.
    */
    long i()
    {
        return m_istart + static_cast<long>(m_li);
    }

    /**
    Customise proximity search region.
    \param distance Width of the region to consider as proximity (default: 10).
    */
    void search_proximity(size_t distance)
    {
        m_proximity = distance;
    }

    /**
    Get the global index of y()(0) = ymin() (the left-most yield position of this chunk).
    \return Index (signed).
    */
    long istart() const
    {
        return m_istart;
    }

    /**
    Get the global index of the slice y() = Y[istart(): istop()].
    \return Index (signed).
    */
    long istop() const
    {
        return m_istop;
    }

    /**
    Get the position of the left-most yield position of the current chunk, ymin() = y()(0).
    \return Yield position.
    */
    double ymin() const
    {
        return m_y.front();
    }

    /**
    Get the position of the right-most yield position of the current chunk, y()(-1) = ymax().
    \return Yield position.
    */
    double ymax() const
    {
        return m_y.back();
    }

    /**
    Get the positions of the left-most yield positions specified in the last chunk.
    Note that ymin_chunk() >= ymin() depending on the buffer.
    Furthermore that when specifying yield distances, the returned value may not be know externally.
    This is why Chunked() keeps track of if.
    */
    double ymin_chunk() const
    {
        return m_ymin_data;
    }

    /**
    Get the entire current chunk of yield positions.
    \return List of yield positions.
    */
    std::vector<double> y() const
    {
        return m_y;
    }

    /**
    Get yield positions along a slice.

    \param start Start index of the slice (global index).
    \param stop Stop index of the slice (global index).
    \return List of yield positions.

    \warning `start` and `stop` must be in the current chunk.
    */
    std::vector<double> Y(long start, long stop) const
    {
        QPOT_ASSERT(start <= stop);
        QPOT_ASSERT(start >= m_istart);
        QPOT_ASSERT(stop <= m_istop);

        size_t i = static_cast<size_t>(start - m_istart);
        size_t j = static_cast<size_t>(stop - m_istart);

        return std::vector<double>(m_y.begin() + i, m_y.begin() + j);
    }

    /**
    Get a specific yield-position.

    \param i Global index.
    \return Yield position.

    \warning `i` must be in the current chunk.
    */
    double Y(long i) const
    {
        QPOT_ASSERT(i >= m_istart);
        QPOT_ASSERT(i < m_istop);

        return m_y[static_cast<size_t>(i - m_istart)];
    }

    /**
    Reset the system: Overwrite the current chunk and global index.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `y[0]`.
    \param y A chunk of yield positions.
    */
    template <class T>
    void set_y(long istart, const T& y)
    {
        QPOT_DEBUG(y.size() > 1);
        QPOT_DEBUG(std::is_sorted(y.cbegin(), y.cend()));

        m_n = y.size();
        m_y.resize(m_n);
        std::copy(y.begin(), y.end(), m_y.begin());

        m_istart = istart;
        m_istop = m_istart + static_cast<long>(m_n);
        m_ymin_data = m_y.front();

        m_full_search = true;
        this->update();
    }

    /**
    Reset the system: Overwrite the current chunk and global index.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param y A chunk of yield positions.
    \param istart The global index of `y[0]`.
    */
    template <class T>
    void set_y(const T& y, long istart = 0)
    {
        this->set_y(istart, y);
    }

    /**
    Right-shift the chunk of yield positions.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `y[0]`.
    \param y A chunk of yield positions (following at least partly the current chunk).
    \param nbuffer Buffer the right `nbuffer` yield positions of the current chunk.

    \note If `istart` < istart() - `nbuffer`,
    more is 'buffered' such that the sequence starts at least at `istart`.

    \note If possible try to keep the size of the chunk constant. Verify that this is the case
    by checking that istop() - istart() is the same before and after this operation.
    */
    template <class T>
    void rshift_y(long istart, const T& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        QPOT_ASSERT(istart <= m_istop);
        QPOT_ASSERT(istart >= m_istart); // otherwise the function does not what it's name suggests

        // if there is overlap between the chunks of Y in memory and input:
        // check that the values are indeed the same
        #ifdef QPOT_ENABLE_DEBUG
            if (istart < m_istop) {
                size_t d = static_cast<size_t>(m_istop - istart);
                xt::xtensor<double, 1> o = xt::empty<double>({d});
                xt::xtensor<double, 1> n = xt::empty<double>({d});
                std::copy(m_y.cend() - d, m_y.cend(), o.begin());
                std::copy(y.cbegin(), y.cbegin() + d, n.begin());
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
        m_ymin_data = *y.cbegin();
        QPOT_ASSERT(nbuffer <= m_n);
        QPOT_DEBUG(std::is_sorted(y.cbegin(), y.cend()));
        size_t d = static_cast<size_t>(m_istop - istart); // items to skip from "y"
        m_istop = istart + static_cast<long>(y.size());
        m_n = static_cast<size_t>(m_istop - m_istart);
        QPOT_ASSERT(m_n >= nbuffer);

        std::copy(m_y.cend() - nbuffer, m_y.cend(), m_y.begin());
        m_y.resize(m_n);
        std::copy(y.cbegin() + d, y.cend(), m_y.begin() + nbuffer);

        m_full_search = true;
        this->update();
    }

    /**
    Right-shift the chunk of yield positions.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param y A chunk of yield positions (following directly the current chunk).
    \param nbuffer Buffer the right `nbuffer` yield positions of the current chunk.

    \warning This function assumes that y[0] has global index istop().
    */
    template <class T>
    void rshift_y(const T& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        this->rshift_y(m_istop, y, nbuffer);
    }

    /**
    Right-shift the chunk of yield positions, using yield distances.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `dy[0]`.
    \param dy The yield distances (following at least partly the current chunk).
    \param nbuffer Buffer a part of the current chunk.
    */
    template <class T>
    void rshift_dy(long istart, const T& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);
        QPOT_ASSERT(istart >= m_istart && istart <= m_istop);
        QPOT_ASSERT(istart + static_cast<long>(dy.size()) >= m_istop);

        std::vector<double> y(dy.size());
        std::partial_sum(dy.cbegin(), dy.cend(), y.begin());

        double d;
        if (istart == m_istop) {
            d = m_y.back();
        }
        else {
            d = m_y.back() - y[m_n - 1 - static_cast<size_t>(istart - m_istart)];
        }
        std::transform(y.begin(), y.end(), y.begin(), [&](auto& v){ return v + d; });

        this->rshift_y(istart, y, nbuffer);
    }

    /**
    Right-shift the chunk of yield positions, using yield distances.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param dy The yield distances following the current right-most yield position.
    \param nbuffer Buffer a part of the current chunk.

    \warning This function assumes that dy[0] has global index istop().
    */
    template <class T>
    void rshift_dy(const T& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);

        std::vector<double> y(dy.size());
        std::partial_sum(dy.cbegin(), dy.cend(), y.begin());

        std::transform(y.begin(), y.end(), y.begin(), [&](auto& v){ return v + m_y.back(); });

        this->rshift_y(m_istop, y, nbuffer);
    }

    /**
    Left-shift the chunk of yield positions.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `y[0]`.
    \param y A chunk of yield positions (preceding at least partly the current chunk).
    \param nbuffer Buffer the left `nbuffer` yield positions of the current chunk.

    \note If possible try to keep the size of the chunk constant. Verify that this is the case
    by checking that istop() - istart() is the same before and after this operation.
    */
    template <class T>
    void lshift_y(long istart, const T& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        long istop = istart + static_cast<long>(y.size());
        QPOT_ASSERT(istart <= m_istart);
        QPOT_ASSERT(istop >= m_istart);

        #ifdef QPOT_ENABLE_DEBUG
            if (istop > m_istart) {
                size_t d = static_cast<size_t>(istop - m_istart);
                xt::xtensor<double, 1> o = xt::empty<double>({d});
                xt::xtensor<double, 1> n = xt::empty<double>({d});
                std::copy(m_y.begin(), m_y.begin() + d, o.begin());
                std::copy(y.end() - d, y.end(), n.begin());
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
        m_ymin_data = *y.cbegin();
        QPOT_ASSERT(nbuffer <= m_n);
        QPOT_DEBUG(std::is_sorted(y.cbegin(), y.cend()));
        size_t d = static_cast<size_t>(istop - m_istart); // items to skip from "y"
        m_istart = istart;
        size_t m_n = static_cast<size_t>(m_istop - m_istart);
        QPOT_ASSERT(m_n >= nbuffer);

        m_y.resize(m_n);
        std::copy(m_y.begin(), m_y.begin() + nbuffer, m_y.end() - nbuffer);
        std::copy(y.cbegin(), y.cend() - d, m_y.begin());

        m_full_search = true;
        this->update();
    }

    /**
    Left-shift the chunk of yield positions.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param y A chunk of yield positions (preceding directly the current chunk).
    \param nbuffer Buffer the left `nbuffer` yield positions of the current chunk.

    \warning This function assumes that y[-1] has global index istart() - 1.
    */
    template <class T>
    void lshift_y(const T& y, size_t nbuffer = 0)
    {
        QPOT_ASSERT(y.size() > 0);
        return this->lshift_y(m_istart - y.size(), y, nbuffer);
    }

    /**
    Left-shift the chunk of yield positions, using yield distances.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `dy[0]`.
    \param dy The yield distances (preceding at least partly the current chunk).
    \param nbuffer Buffer a part of the current chunk.
    */
    template <class T>
    void lshift_dy(long istart, const T& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);
        QPOT_ASSERT(istart + static_cast<long>(dy.size()) > m_istart);

        std::vector<double> y(dy.size());
        std::partial_sum(dy.cbegin(), dy.cend(), y.begin());

        double d = m_y[istart + static_cast<long>(y.size()) - m_istart - 1] - y.back();
        std::transform(y.begin(), y.end(), y.begin(), [&](auto& v){ return v + d; });

        this->lshift_y(istart, y, nbuffer);
    }

    /**
    Left-shift the chunk of yield positions, using yield distances.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param dy The yield distances preceding the current left-most yield position including istart().
    \param nbuffer Buffer a part of the current chunk.

    \warning This function assumes that dy[-1] has global index istart().
    */
    template <class T>
    void lshift_dy(const T& dy, size_t nbuffer = 0)
    {
        QPOT_ASSERT(dy.size() > 0);

        std::vector<double> y(dy.size());
        std::partial_sum(dy.cbegin(), dy.cend(), y.begin());

        double d = m_y.front() - y.back();
        std::transform(y.cbegin(), y.cend(), y.begin(), [&](auto& v){ return v + d; });

        this->lshift_y(m_istart - static_cast<long>(y.size()) + 1, y, nbuffer);
    }

    /**
    Shift chunk (left or right, but there may not be any holes compared to the current chunk).

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `dy[0]`.
    \param y The yield positions.
    \param nbuffer Buffer a part of the current chunk.
    */
    template <class T>
    void shift_y(long istart, const T& y, size_t nbuffer = 0)
    {
        if (istart < m_istart) {
            return this->lshift_y(istart, y, nbuffer);
        }
        return this->rshift_y(istart, y, nbuffer);
    }

    /**
    Shift chunk chunk of yield positions.
    The shift may be left or right, but there may not be any holes compared to the current chunk.

    \tparam T The type of the contain `y`. Should implement `begin()` and `end()``.
    \param istart The global index of `dy[0]`.
    \param dy The yield distances.
    \param nbuffer Buffer a part of the current chunk.
    */
    template <class T>
    void shift_dy(long istart, const T& dy, size_t nbuffer = 0)
    {
        if (istart < m_istart) {
            return this->lshift_dy(istart, dy, nbuffer);
        }
        return this->rshift_dy(istart, dy, nbuffer);
    }

    /**
    The current position.
    \return Position.
    */
    double x() const
    {
        return m_x;
    }

    /**
    Set the current position.
    \param x The current position.
    */
    void set_x(double x)
    {
        m_x = x;
        this->update();
    }

    /**
    Check for redraw:
    -   0: No redraw needed.
    -   -1: To the left.
    -   +1: To the right.

    \return Redraw direction
    */
    int redraw() const
    {
        return m_redraw;
    }

    /**
    Yield position directly left of x().
    Same as y()[i() - istart()].

    \return Yield position.
    */
    double yleft() const
    {
        QPOT_ASSERT(!m_lock);
        return m_left;
    }

    /**
    Yield position directly right of x().
    Same as y()[i() - istart() + 1].

    \return Yield position.
    */
    double yright() const
    {
        QPOT_ASSERT(!m_lock);
        return m_right;
    }

    /**
    Yield position at an offset left of x().
    Same as y()[i() - istart() - offset].

    \param offset The offset.
    \return Yield position.
    */
    double yleft(size_t offset) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(offset <= m_li);
        return m_y[m_li - offset];
    }

    /**
    Yield position at an offset right of x().
    Same as y()[i() - istart() + 1 + offset].

    \param offset The offset.
    \return Yield position.
    */
    double yright(size_t offset) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(m_li + 1 + offset < m_n);
        return m_y[m_li + 1 + offset];
    }

    /**
    Check that x() is at least `n` yield positions from the far-left.

    \param n Offset.
    */
    bool boundcheck_left(size_t n = 0) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(n < m_n);
        return m_li > n;
    }

    /**
    Check that x() is at least `n` yield positions from the far-right.

    \param n Offset.
    */
    bool boundcheck_right(size_t n = 0) const
    {
        QPOT_ASSERT(!m_lock);
        QPOT_ASSERT(n < m_n);
        return m_li < m_n - n;
    }

private:

    void update()
    {
        if (m_x <= m_y.front()) {
            m_redraw = -1;
        }
        else if (m_x > m_y.back()) {
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
            m_li = std::lower_bound(m_y.cbegin(), m_y.cend(), m_x) - m_y.cbegin() - 1;
            m_left = m_y[m_li];
            m_right = m_y[m_li + 1];
            m_full_search = false;
            return;
        }

        if (m_left < m_x && m_right >= m_x) {
            return;
        }

        size_t l = m_li > m_proximity ? m_li - m_proximity : 0;
        size_t r = std::min(m_li + m_proximity, m_n - 1);

        if (m_y[l] < m_x && m_y[r] >= m_x) {
            m_li = std::lower_bound(&m_y[l], &m_y[l] + r - l, m_x) - &m_y[l] - 1 + l;
        }
        else {
            m_li = std::lower_bound(m_y.cbegin(), m_y.cend(), m_x) - m_y.cbegin() - 1;
        }

        m_left = m_y[m_li];
        m_right = m_y[m_li + 1];
    }

private:

    double m_x = 0.0; ///< Current positions
    size_t m_li; ///< Local index for the position of m_x in m_y
    size_t m_n; ///< Number of yield positions
    size_t m_proximity = 10; ///< See search_proximity()
    long m_istart; ///< See istart()
    long m_istop; ///< See istop()
    int m_redraw; ///< See redraw()
    bool m_lock = true; ///< If `true` the m_x is out-of-bounds for the current chuck.
    bool m_full_search = true; ///< Update m_li by performing a global search on the chunk.
    double m_left; ///< See yleft()
    double m_right; ///< See yright()
    std::vector<double> m_y; ///< Current chunk of yield positions
    double m_ymin_data = 0.0; ///< The value of ymin() of the supplied data (excl. buffer)

}; // class Chunked

} // namespace QPot

#endif






