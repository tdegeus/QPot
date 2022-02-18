#define CATCH_CONFIG_MAIN // tells Catch to provide a main() - only do this in one cpp file
#include <QPot/Chunked.hpp>
#include <catch2/catch.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xtensor.hpp>
#include <xtensor/xview.hpp>

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

        REQUIRE(chunk.size() == y.size());
        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_right());
        REQUIRE(chunk.ymin_chunk() == y(0));

        REQUIRE(xt::allclose(chunk.yleft(), 5.0));
        REQUIRE(xt::allclose(chunk.yright(), 6.0));

        REQUIRE(xt::allclose(chunk.yleft(0), chunk.yleft()));
        REQUIRE(xt::allclose(chunk.yleft(1), 4.0));
        REQUIRE(xt::allclose(chunk.yleft(2), 3.0));

        REQUIRE(xt::allclose(chunk.yright(0), chunk.yright()));
        REQUIRE(xt::allclose(chunk.yright(1), 7.0));
        REQUIRE(xt::allclose(chunk.yright(2), 8.0));

        REQUIRE(xt::allclose(chunk.yright(0), chunk.yright()));
        REQUIRE(xt::allclose(chunk.yright(1), 7.0));
        REQUIRE(xt::allclose(chunk.yright(2), 8.0));

        REQUIRE(xt::allclose(chunk.Y(chunk.i() - 0), chunk.yleft()));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() - 1), 4.0));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() - 2), 3.0));

        REQUIRE(xt::allclose(chunk.Y(chunk.i() + 1), chunk.yright()));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() + 2), 7.0));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() + 3), 8.0));

        REQUIRE(xt::allclose(xt::adapt(chunk.Y(0, 11)), y));

        chunk.set_x(6.5);

        REQUIRE(chunk.i() == 6);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_right());
        REQUIRE(chunk.ymin_chunk() == y(0));

        REQUIRE(xt::allclose(chunk.yleft(), 6.0));
        REQUIRE(xt::allclose(chunk.yright(), 7.0));

        REQUIRE(xt::allclose(chunk.yleft(0), chunk.yleft()));
        REQUIRE(xt::allclose(chunk.yleft(1), 5.0));
        REQUIRE(xt::allclose(chunk.yleft(2), 4.0));

        REQUIRE(xt::allclose(chunk.yright(0), chunk.yright()));
        REQUIRE(xt::allclose(chunk.yright(1), 8.0));
        REQUIRE(xt::allclose(chunk.yright(2), 9.0));

        REQUIRE(xt::allclose(chunk.Y(chunk.i() - 0), chunk.yleft()));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() - 1), 5.0));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() - 2), 4.0));

        REQUIRE(xt::allclose(chunk.Y(chunk.i() + 1), chunk.yright()));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() + 2), 8.0));
        REQUIRE(xt::allclose(chunk.Y(chunk.i() + 3), 9.0));
    }

    SECTION("Bounds check")
    {
        xt::xtensor<double, 1> y = xt::linspace<double>(0, 5, 6);

        QPot::Chunked chunk(0.5, y);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_right());

        chunk.set_x(-0.5);

        REQUIRE(!chunk.boundcheck_left());
        REQUIRE(!chunk.boundcheck_right());

        chunk.set_x(0.5);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(!chunk.boundcheck_left(1));
        REQUIRE(!chunk.boundcheck_left(2));
        REQUIRE(!chunk.boundcheck_left(3));
        REQUIRE(!chunk.boundcheck_left(4));
        REQUIRE(!chunk.boundcheck_left(5));

        REQUIRE(chunk.boundcheck_right());
        REQUIRE(chunk.boundcheck_right(1));
        REQUIRE(chunk.boundcheck_right(2));
        REQUIRE(chunk.boundcheck_right(3));
        REQUIRE(chunk.boundcheck_right(4));
        REQUIRE(!chunk.boundcheck_right(5));

        chunk.set_x(1.5);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_left(1));
        REQUIRE(!chunk.boundcheck_left(2));
        REQUIRE(!chunk.boundcheck_left(3));
        REQUIRE(!chunk.boundcheck_left(4));
        REQUIRE(!chunk.boundcheck_left(5));

        REQUIRE(chunk.boundcheck_right());
        REQUIRE(chunk.boundcheck_right(1));
        REQUIRE(chunk.boundcheck_right(2));
        REQUIRE(chunk.boundcheck_right(3));
        REQUIRE(!chunk.boundcheck_right(4));
        REQUIRE(!chunk.boundcheck_right(5));

        chunk.set_x(2.5);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_left(1));
        REQUIRE(chunk.boundcheck_left(2));
        REQUIRE(!chunk.boundcheck_left(3));
        REQUIRE(!chunk.boundcheck_left(4));
        REQUIRE(!chunk.boundcheck_left(5));

        REQUIRE(chunk.boundcheck_right());
        REQUIRE(chunk.boundcheck_right(1));
        REQUIRE(chunk.boundcheck_right(2));
        REQUIRE(!chunk.boundcheck_right(3));
        REQUIRE(!chunk.boundcheck_right(4));
        REQUIRE(!chunk.boundcheck_right(5));

        chunk.set_x(3.5);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_left(1));
        REQUIRE(chunk.boundcheck_left(2));
        REQUIRE(chunk.boundcheck_left(3));
        REQUIRE(!chunk.boundcheck_left(4));
        REQUIRE(!chunk.boundcheck_left(5));

        REQUIRE(chunk.boundcheck_right());
        REQUIRE(chunk.boundcheck_right(1));
        REQUIRE(!chunk.boundcheck_right(2));
        REQUIRE(!chunk.boundcheck_right(3));
        REQUIRE(!chunk.boundcheck_right(4));
        REQUIRE(!chunk.boundcheck_right(5));

        chunk.set_x(4.5);

        REQUIRE(chunk.boundcheck_left());
        REQUIRE(chunk.boundcheck_left(1));
        REQUIRE(chunk.boundcheck_left(2));
        REQUIRE(chunk.boundcheck_left(3));
        REQUIRE(chunk.boundcheck_left(4));
        REQUIRE(!chunk.boundcheck_left(5));

        REQUIRE(chunk.boundcheck_right());
        REQUIRE(!chunk.boundcheck_right(1));
        REQUIRE(!chunk.boundcheck_right(2));
        REQUIRE(!chunk.boundcheck_right(3));
        REQUIRE(!chunk.boundcheck_right(4));
        REQUIRE(!chunk.boundcheck_right(5));

        chunk.set_x(5.5);

        REQUIRE(!chunk.boundcheck_left());
        REQUIRE(!chunk.boundcheck_right());
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
        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        chunk.set_x(13.5);
        REQUIRE(chunk.redraw() == +1);

        istart = chunk.istop();
        istop = static_cast<decltype(istop)>(dy.size());

        chunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istop))));
        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = 0;
        istop = 21;

        chunk.rshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))), 3);
        dhunk.rshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, istop))), 3);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), y));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(xt::adapt(dhunk.y()), y));

        chunk.rshift_y(1, xt::eval(xt::view(y, xt::range(1, 21)))); // should be ignored
        chunk.rshift_dy(1, xt::eval(xt::view(dy, xt::range(1, 21)))); // should be ignored

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), y));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(xt::adapt(dhunk.y()), y));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 21;

        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istop))));

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 20;

        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istop))));

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 21;
        size_t nbuffer = 3;

        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istop))), nbuffer);
        dhunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istop))), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 17;
        size_t nbuffer = 3;

        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istop))), nbuffer);
        dhunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istop))), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 21;
        size_t nbuffer = 3;

        chunk.rshift_y(istart - 2, xt::eval(xt::view(y, xt::range(istart - 2, istop))), nbuffer);
        dhunk.rshift_dy(istart - 2, xt::eval(xt::view(dy, xt::range(istart - 2, istop))), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart - 2)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.redraw() == +1);
        REQUIRE(dhunk.redraw() == +1);

        istart = chunk.istop();
        istop = 17;
        size_t nbuffer = 3;

        chunk.rshift_y(istart - 2, xt::eval(xt::view(y, xt::range(istart - 2, istop))), nbuffer);
        dhunk.rshift_dy(istart - 2, xt::eval(xt::view(dy, xt::range(istart - 2, istop))), nbuffer);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart - static_cast<decltype(istart)>(nbuffer));
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart - 2)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
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

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istart = 0;
        istop = 21;

        chunk.lshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))), 3);
        dhunk.lshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, istop))), 3);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), y));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(xt::adapt(dhunk.y()), y));

        chunk.lshift_y(istart, xt::eval(xt::view(y, xt::range(istart, 20)))); // should be ignored
        chunk.lshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, 20)))); // should be ignored

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), y));

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(xt::adapt(dhunk.y()), y));
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

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 0;

        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.lshift_dy(xt::eval(xt::view(dy, xt::range(istart, istop + 1))));

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop + 1);
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 3;

        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.lshift_dy(xt::eval(xt::view(dy, xt::range(istart + 1, istop + 1))));

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart + 1);
        REQUIRE(dhunk.istop() == istop + 1);
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 0;
        size_t nbuffer = 3;

        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istop))), nbuffer);
        dhunk.lshift_dy(xt::eval(xt::view(dy, xt::range(istart + 1, istop + 1))), nbuffer);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart + 1);
        REQUIRE(dhunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 1;

        chunk.lshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.lshift_dy(istart + 1, xt::eval(xt::view(dy, xt::range(istart + 1, istop + 1))));

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() == n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart + 1);
        REQUIRE(dhunk.istop() == istop + 1);
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
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

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));
        dhunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))));

        chunk.set_x(13.5);
        dhunk.set_x(13.5);

        auto n = chunk.istop() - chunk.istart();

        REQUIRE(chunk.i() == 13);
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop);
        REQUIRE(chunk.ymin_chunk() == Approx(y(istart)));
        REQUIRE(chunk.ymin() == Approx(y(chunk.istart())));
        REQUIRE(chunk.ymax() == Approx(y(chunk.istop() - 1)));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));

        REQUIRE(dhunk.i() == 13);
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));

        chunk.set_x(5.5);
        dhunk.set_x(5.5);

        REQUIRE(chunk.redraw() == -1);
        REQUIRE(dhunk.redraw() == -1);

        istop = chunk.istart();
        istart = 0;
        size_t nbuffer = 3;

        chunk.lshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istop))), nbuffer);
        dhunk.lshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, istop + 1))), nbuffer);

        REQUIRE(chunk.i() == 5);
        REQUIRE(chunk.ymin_chunk() == y(istart));
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(chunk.redraw() == 0);
        REQUIRE(chunk.istart() == istart);
        REQUIRE(chunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(
            xt::adapt(chunk.y()), xt::view(y, xt::range(chunk.istart(), chunk.istop()))));
        REQUIRE(chunk.istop() - chunk.istart() != n);

        REQUIRE(dhunk.i() == 5);
        REQUIRE(chunk.ymin_chunk() == y(istart));
        REQUIRE(chunk.yleft() == Approx(y(chunk.i())));
        REQUIRE(chunk.yright() == Approx(y(chunk.i() + 1)));
        REQUIRE(dhunk.redraw() == 0);
        REQUIRE(dhunk.ymin_chunk() == y(istart));
        REQUIRE(dhunk.istart() == istart);
        REQUIRE(dhunk.istop() == istop + static_cast<decltype(istart)>(nbuffer));
        REQUIRE(xt::allclose(
            xt::adapt(dhunk.y()), xt::view(y, xt::range(dhunk.istart(), dhunk.istop()))));
        REQUIRE(dhunk.istop() - dhunk.istart() != n);
    }

    SECTION("example from Chunk documentation - intro")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop();
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(5, 10))));

        istart = chunk.istop();
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(10, 15))));
    }

    SECTION("example from Chunk documentation - buffer")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop(); // 5
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(3, 10))));

        istart = chunk.istop(); // 10
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(8, 15))));
    }

    SECTION("example from Chunk documentation - distances")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop(); // 5
        chunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(3, 10))));

        istart = chunk.istop(); // 10
        chunk.rshift_dy(xt::eval(xt::view(dy, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(8, 15))));
    }

    SECTION("example from Chunk documentation - left (1)")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop(); // 5
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(5, 10))));

        istart = chunk.istop(); // 10
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(10, 15))));

        istart = 5;
        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(5, 10))));

        istart = 0;
        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));
    }

    SECTION("example from Chunk documentation - left (2)")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop(); // 5
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(3, 10))));

        istart = chunk.istop(); // 10
        chunk.rshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(8, 15))));

        istart = 5;
        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 3))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(5, 10))));

        istart = 0;
        chunk.lshift_y(xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 7))));
    }

    SECTION("example from Chunk documentation - left (3)")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop(); // 5
        chunk.rshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(3, 10))));

        istart = chunk.istop(); // 10
        chunk.rshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(8, 15))));

        istart = 5;
        chunk.lshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(5, 10))));

        istart = 0;
        chunk.lshift_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 7))));
    }

    SECTION("example from Chunk documentation - left (4)")
    {
        xt::xtensor<double, 1> dy = xt::ones<double>({21});
        dy(0) = 0.0;
        xt::view(dy, xt::range(0, 21, 2)) += 0.03 * xt::arange<double>(11);
        xt::view(dy, xt::range(1, 21, 2)) -= 0.02 * xt::arange<double>(10);

        xt::xtensor<double, 1> y = xt::cumsum(dy);

        QPot::Chunked chunk;
        long istart = 0;

        chunk.set_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 5))));
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 5))));

        istart = chunk.istop(); // 5
        chunk.rshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(3, 10))));

        istart = chunk.istop(); // 10
        chunk.rshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(8, 15))));

        istart = 5;
        chunk.lshift_dy(istart, xt::eval(xt::view(dy, xt::range(istart, istart + 5))), 2);
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(5, 10))));

        istart = 0;
        chunk.lshift_dy(
            istart, xt::eval(xt::view(dy, xt::range(istart, istart + 5 + 1))), 2); // careful here
        REQUIRE(xt::allclose(xt::adapt(chunk.y()), xt::view(y, xt::range(0, 7))));
    }

    SECTION("random, minimal buffer, fixed memory")
    {
        xt::xtensor<double, 1> dy = xt::random::rand<double>({1001});
        dy(0) = -dy(0);
        xt::xtensor<double, 1> y = xt::cumsum(dy);
        xt::xtensor<double, 1> x = 0.99 * xt::amax(y)() * xt::random::rand<double>({100});
        x = xt::sort(x);

        QPot::Chunked chunk(0.0, xt::eval(xt::view(y, xt::range(0, 101))));
        size_t n = chunk.y().size();

        for (auto& xi : x) {
            chunk.set_x(xi);
            while (chunk.redraw() == +1) {
                auto istart = chunk.istop();
                chunk.shift_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 100))), 1);
                REQUIRE(chunk.y().size() == n);
                REQUIRE(chunk.istop() - chunk.istart() == static_cast<long>(n));
            }
            REQUIRE(((chunk.yleft() < xi) && (xi <= chunk.yright())));
            REQUIRE(chunk.y().size() == n);
            REQUIRE(chunk.istop() - chunk.istart() == static_cast<long>(n));
        }

        std::reverse(x.begin(), x.end());

        for (auto& xi : x) {
            chunk.set_x(xi);
            while (chunk.redraw() == -1) {
                auto istart = chunk.istart() - 100;
                chunk.shift_y(istart, xt::eval(xt::view(y, xt::range(istart, istart + 101))), 1);
                REQUIRE(chunk.y().size() == n);
                REQUIRE(chunk.istop() - chunk.istart() == static_cast<long>(n));
            }
            REQUIRE(((chunk.yleft() < xi) && (xi <= chunk.yright())));
            REQUIRE(chunk.y().size() == n);
            REQUIRE(chunk.istop() - chunk.istart() == static_cast<long>(n));
        }
    }
}
