#include <catch2/catch.hpp>
#include <QPot/Chunked.hpp>
#include <xtensor/xio.hpp>

TEST_CASE("QPot::Chunked", "Chunked.hpp")
{
    SECTION("Basic indexing")
    {
        xt::xtensor<double, 1> y = xt::linspace<double>(0, 100, 101);

        QPot::Chunked chunk(5.5, y);
        REQUIRE(chunk.i() == 5);

        chunk.set_x(5.6);
        REQUIRE(chunk.i() == 5);

        chunk.set_x(5.7);
        REQUIRE(chunk.i() == 5);

        chunk.set_x(6.5);
        REQUIRE(chunk.i() == 6);

        chunk.set_x(91.5);
        REQUIRE(chunk.i() == 91);
    }

    SECTION("Basic behaviour")
    {
        xt::xtensor<double, 1> y = xt::linspace<double>(0, 10, 11);

        QPot::Chunked chunk(5.5, y);

        REQUIRE(chunk.i() == 5);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_right());

        REQUIRE(xt::allclose(chunk.yi_left(), 5.0));
        REQUIRE(xt::allclose(chunk.yi_right(), 6.0));

        REQUIRE(xt::allclose(chunk.yi_left(0), chunk.yi_left()));
        REQUIRE(xt::allclose(chunk.yi_left(1), 4.0));
        REQUIRE(xt::allclose(chunk.yi_left(2), 3.0));

        REQUIRE(xt::allclose(chunk.yi_right(0), chunk.yi_right()));
        REQUIRE(xt::allclose(chunk.yi_right(1), 7.0));
        REQUIRE(xt::allclose(chunk.yi_right(2), 8.0));

        REQUIRE(xt::allclose(chunk.yi_right(0), chunk.yi_right()));
        REQUIRE(xt::allclose(chunk.yi_right(1), 7.0));
        REQUIRE(xt::allclose(chunk.yi_right(2), 8.0));

        REQUIRE(xt::allclose(chunk.yi(-1), chunk.yi_left()));
        REQUIRE(xt::allclose(chunk.yi(-2), 4.0));
        REQUIRE(xt::allclose(chunk.yi(-3), 3.0));

        REQUIRE(xt::allclose(chunk.yi(+1), chunk.yi_right()));
        REQUIRE(xt::allclose(chunk.yi(+2), 7.0));
        REQUIRE(xt::allclose(chunk.yi(+3), 8.0));

        chunk.set_x(6.5);

        REQUIRE(chunk.i() == 6);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_right());

        REQUIRE(xt::allclose(chunk.yi_left(), 6.0));
        REQUIRE(xt::allclose(chunk.yi_right(), 7.0));

        REQUIRE(xt::allclose(chunk.yi_left(0), chunk.yi_left()));
        REQUIRE(xt::allclose(chunk.yi_left(1), 5.0));
        REQUIRE(xt::allclose(chunk.yi_left(2), 4.0));

        REQUIRE(xt::allclose(chunk.yi_right(0), chunk.yi_right()));
        REQUIRE(xt::allclose(chunk.yi_right(1), 8.0));
        REQUIRE(xt::allclose(chunk.yi_right(2), 9.0));

        REQUIRE(xt::allclose(chunk.yi(-1), chunk.yi_left()));
        REQUIRE(xt::allclose(chunk.yi(-2), 5.0));
        REQUIRE(xt::allclose(chunk.yi(-3), 4.0));

        REQUIRE(xt::allclose(chunk.yi(+1), chunk.yi_right()));
        REQUIRE(xt::allclose(chunk.yi(+2), 8.0));
        REQUIRE(xt::allclose(chunk.yi(+3), 9.0));
    }

    SECTION("Basic chunk features")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;

        long istart = 0;
        long istop = 10;
        chunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        chunk.set_x(13.5);
        REQUIRE(chunk.redraw() == +1);

        istart = chunk.istop();
        istop = static_cast<decltype(istop)>(dy.size());

        chunk.rshift_dy(xt::view(dy, xt::range(istart, istop)));
        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
    }

    SECTION("rshift_y, extend")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = 0;
        istop = 21;

        chunk.rshift_y(istart, xt::view(y, xt::range(istart, istop)), 3);
        dhunk.rshift_dy(istart, xt::view(dy, xt::range(istart, istop)), 3);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), y));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), y));

        chunk.rshift_y(1, xt::view(y, xt::range(1, 21))); // should be ignored
        chunk.rshift_dy(1, xt::view(dy, xt::range(1, 21))); // should be ignored

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), y));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), y));
    }

    SECTION("rshift_y, shift, internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 21;

        chunk.rshift_y(xt::view(y, xt::range(istart, istop)));
        dhunk.rshift_dy(xt::view(dy, xt::range(istart, istop)));

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }

    SECTION("rshift_y, shift, no internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 20;

        chunk.rshift_y(xt::view(y, xt::range(istart, istop)));
        dhunk.rshift_dy(xt::view(dy, xt::range(istart, istop)));

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() == n);
    }

    SECTION("rshift_y, buffer, internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 21;
        size_t nbuffer = 3;

        chunk.rshift_y(xt::view(y, xt::range(istart, istop)), nbuffer);
        dhunk.rshift_dy(xt::view(dy, xt::range(istart, istop)), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }

    SECTION("rshift_y, buffer, no internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 17;
        size_t nbuffer = 3;

        chunk.rshift_y(xt::view(y, xt::range(istart, istop)), nbuffer);
        dhunk.rshift_dy(xt::view(dy, xt::range(istart, istop)), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() == n);
    }

    SECTION("rshift_y, buffer, internal resize, istart")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 21;
        size_t nbuffer = 3;

        chunk.rshift_y(istart - 2, xt::view(y, xt::range(istart - 2, istop)), nbuffer);
        dhunk.rshift_dy(istart - 2, xt::view(dy, xt::range(istart - 2, istop)), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }

    SECTION("rshift_y, buffer, no internal resize, istart")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 0;
        long istop = 10;

        chunk.set_y(xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(xt::view(y, xt::range(istart, istop)));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 17;
        size_t nbuffer = 3;

        chunk.rshift_y(istart - 2, xt::view(y, xt::range(istart - 2, istop)), nbuffer);
        dhunk.rshift_dy(istart - 2, xt::view(dy, xt::range(istart - 2, istop)), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);
    }

    SECTION("lshift_y, extend")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 11;
        long istop = 21;

        chunk.set_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(istart, xt::view(y, xt::range(istart, istop)));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istart = 0;
        istop = 21;

        chunk.lshift_y(istart, xt::view(y, xt::range(istart, istop)), 3);
        dhunk.lshift_dy(istart, xt::view(dy, xt::range(istart, istop)), 3);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), y));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), y));

        chunk.lshift_y(istart, xt::view(y, xt::range(istart, 20))); // should be ignored
        chunk.lshift_dy(istart, xt::view(dy, xt::range(istart, 20))); // should be ignored

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), y));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), y));
    }

    SECTION("lshift_y, shift, internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 11;
        long istop = 21;

        chunk.set_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(istart, xt::view(y, xt::range(istart, istop)));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 0;

        chunk.lshift_y(xt::view(y, xt::range(istart, istop)));
        dhunk.lshift_dy(xt::view(dy, xt::range(istart, istop + 1)));

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop + 1);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }

    SECTION("lshift_y, shift, no internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 12;
        long istop = 21;

        chunk.set_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(istart, xt::view(y, xt::range(istart, istop)));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 3;

        chunk.lshift_y(xt::view(y, xt::range(istart, istop)));
        dhunk.lshift_dy(xt::view(dy, xt::range(istart + 1, istop + 1)));

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart + 1);
        REQUIRE(dhunk.istop() == istop + 1);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() == n);
    }

    SECTION("lshift_y, buffer, internal resize")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 11;
        long istop = 21;

        chunk.set_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(istart, xt::view(y, xt::range(istart, istop)));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 0;
        size_t nbuffer = 3;

        chunk.lshift_y(xt::view(y, xt::range(istart, istop)), nbuffer);
        dhunk.lshift_dy(xt::view(dy, xt::range(istart + 1, istop + 1)), nbuffer);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart + 1);
        REQUIRE(dhunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }

    SECTION("lshift_y, shift, no internal resize, istart")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 11;
        long istop = 21;

        chunk.set_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(istart, xt::view(y, xt::range(istart, istop)));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 1;

        chunk.lshift_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.lshift_dy(istart + 1, xt::view(dy, xt::range(istart + 1, istop + 1)));

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart + 1);
        REQUIRE(dhunk.istop() == istop + 1);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() == n);
    }

    SECTION("lshift_y, buffer, internal resize, istart")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        QPot::Chunked dhunk; // chunk in terms of "dy"

        long istart = 11;
        long istop = 21;

        chunk.set_y(istart, xt::view(y, xt::range(istart, istop)));
        dhunk.set_y(istart, xt::view(y, xt::range(istart, istop)));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 0;
        size_t nbuffer = 3;

        chunk.lshift_y(istart, xt::view(y, xt::range(istart, istop)), nbuffer);
        dhunk.lshift_dy(istart, xt::view(dy, xt::range(istart, istop + 1)), nbuffer);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(chunk.y(), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(dhunk.y(), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }






































    // SECTION("rshift_dy, extend")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(0, 10)));

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.i() == 5);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 10);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.redraw() == +1);

    //     chunk.rshift_dy(0, xt::view(dy, xt::range(0, 21)), 3);
    //     REQUIRE(chunk.i() == 13);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 21);
    //     REQUIRE(chunk.redraw() == 0);
    //     REQUIRE(xt::allclose(chunk.y(), y));

    //     chunk.rshift_dy(1, xt::view(dy, xt::range(1, 21)));
    //     REQUIRE(chunk.i() == 13);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 21);
    //     REQUIRE(chunk.redraw() == 0);
    //     REQUIRE(xt::allclose(chunk.y(), y));
    // }

    // SECTION("rshift_dy, shift, internal resize")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(0, 10)));

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.i() == 5);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.redraw() == +1);

    //     chunk.rshift_dy(xt::view(dy, xt::range(11, 21)), 3);
    //     REQUIRE(chunk.i() == 13);
    //     REQUIRE(chunk.redraw() == 0);
    // }

    // SECTION("rshift_dy, shift, no internal resize")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(0, 10)));

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.i() == 5);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.redraw() == +1);

    //     chunk.rshift_dy(xt::view(dy, xt::range(11, 18)), 3);
    //     REQUIRE(chunk.i() == 13);
    //     REQUIRE(chunk.redraw() == 0);
    // }

    // SECTION("rshift_dy, buffer, internal resize")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(0, 10)));

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.i() == 5);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 10);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.redraw() == +1);

    //     chunk.rshift_dy(8, xt::view(dy, xt::range(8, 15)), 3);
    //     REQUIRE(chunk.i() == 13);
    //     REQUIRE(chunk.istart() == 7);
    //     REQUIRE(chunk.istop() == 15);
    //     REQUIRE(chunk.redraw() == 0);
    // }

    // SECTION("rshift_dy, buffer, no internal resize")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(0, 10)));

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.i() == 5);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 10);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.redraw() == +1);

    //     chunk.rshift_dy(8, xt::view(dy, xt::range(8, 17)), 3);
    //     REQUIRE(chunk.i() == 13);
    //     REQUIRE(chunk.istart() == 7);
    //     REQUIRE(chunk.istop() == 17);
    //     REQUIRE(chunk.redraw() == 0);
    // }

    // SECTION("lshift_y")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(10, 21)), 10);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.i() == 13);

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.redraw() == -1);

    //     chunk.lshift_y(xt::view(y, xt::range(0, 10)));
    //     REQUIRE(chunk.i() == 5);
    //     REQUIRE(chunk.redraw() == 0);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 10);
    // }

    // SECTION("lshift_y, extend")
    // {
    //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 20, 21);
    //     xt::xtensor<double, 1> dy = xt::ones<double>(y.shape());
    //     dy(0) = y(0);

    //     QPot::Chunked chunk;
    //     chunk.set_y(xt::view(y, xt::range(10, 21)), 10);

    //     chunk.set_x(13.5);
    //     REQUIRE(chunk.i() == 13);

    //     chunk.set_x(5.5);
    //     REQUIRE(chunk.redraw() == -1);

    //     chunk.lshift_y(0, y);
    //     REQUIRE(chunk.i() == 5);
    //     REQUIRE(chunk.redraw() == 0);
    //     REQUIRE(chunk.istart() == 0);
    //     REQUIRE(chunk.istop() == 21);
    // }


    // SECTION("Basic chunk restore features")
    // {
    //     // QPot::Chunked yield;
    //     // yield.set_dy(0.0, xt::ones<double>({11}));
    //     // yield.set_x(5.5);
    //     // REQUIRE(yield.i() == 5);

    //     // yield.set_x(13.7);
    //     // REQUIRE(yield.redraw() == +1);

    //     // double imax = yield.imax();
    //     // double ymax = yield.ymax();
    //     // yield.set_dy(ymax, 0.5 * xt::ones<double>({40}), imax);
    //     // REQUIRE(yield.i() == 17);
    //     // REQUIRE(yield.redraw() == 0);

    //     // yield.set_x(14.7);
    //     // REQUIRE(yield.i() == 19);
    //     // REQUIRE(yield.redraw() == 0);

    //     // yield.lshift_dy(ymax, xt::ones<double>({11}));
    //     // yield.set_x(5.5);
    //     // REQUIRE(yield.i() == 5);
    //     // REQUIRE(yield.imin() == 0);
    //     // REQUIRE(yield.imax() == 11);
    // }

    // // SECTION("Chunked::currentYield")
    // // {
    // //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 10, 11);
    // //     QPot::Chunked yield(5.5, y);
    // //     xt::xtensor<double, 1> ret = {3, 4, 5, 6, 7};
    // //     REQUIRE(xt::allclose(yield.currentYield(-2, 3), ret));
    // // }

    // // SECTION("Chunked::yield - index")
    // // {
    // //     xt::xtensor<double, 1> y = xt::linspace<double>(0, 10, 11);
    // //     QPot::Chunked yield(5.5, y);
    // //     REQUIRE(xt::allclose(yield.yieldPosition(0), 0.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(1), 1.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(2), 2.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(3), 3.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(4), 4.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(5), 5.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(6), 6.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(7), 7.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(8), 8.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(9), 9.0));
    // //     REQUIRE(xt::allclose(yield.yieldPosition(10), 10.0));
    // // }
}
