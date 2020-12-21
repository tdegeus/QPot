
#include <catch2/catch.hpp>
#include <QPot/Redraw.hpp>
#include <xtensor/xio.hpp>

TEST_CASE("QPot::RedrawList", "Redraw.hpp")
{
    SECTION("Basic behaviour")
    {
        xt::xtensor<double, 1> x = {-1.0, 0.0, 1.0};
        xt::xtensor<double, 1> l = {-1.5, -0.5, 0.5};
        xt::xtensor<double, 1> r = l + 1.0;
        xt::xtensor<long, 1> i = {13, 14, 15};

        auto uniform = [=](std::vector<size_t> shape) { return xt::ones<double>(shape); };

        QPot::RedrawList yield(x, uniform, 30, 5, 2);

        for (size_t j = 0; j < 20; ++j) {
            x += 1.0;
            l += 1.0;
            r += 1.0;
            i += 1;
            yield.setPosition(x);

            REQUIRE(xt::all(xt::equal(yield.currentIndex(), i)));

            REQUIRE(xt::allclose(yield.currentYieldLeft(), l));
            REQUIRE(xt::allclose(yield.currentYieldRight(), r));

            REQUIRE(xt::allclose(yield.currentYieldLeft(0), l));
            REQUIRE(xt::allclose(yield.currentYieldRight(0), r));

            REQUIRE(xt::allclose(yield.nextYield(-1), l));
            REQUIRE(xt::allclose(yield.nextYield(+1), r));

            if (xt::amin(yield.raw_idx())() > 0) {
                REQUIRE(xt::allclose(yield.currentYieldLeft(1), l - 1.0));
                REQUIRE(xt::allclose(yield.nextYield(-2), l - 1.0));
            }
            if (xt::amin(yield.raw_idx())() > 1) {
                REQUIRE(xt::allclose(yield.currentYieldLeft(2), l - 2.0));
                REQUIRE(xt::allclose(yield.nextYield(-3), l - 2.0));
            }

            if (xt::amax(yield.raw_idx())() < 30 - 2) {
                REQUIRE(xt::allclose(yield.currentYieldRight(1), r + 1.0));
                REQUIRE(xt::allclose(yield.nextYield(+2), r + 1.0));
            }
            if (xt::amax(yield.raw_idx())() < 30 - 3) {
                REQUIRE(xt::allclose(yield.currentYieldRight(2), r + 2.0));
                REQUIRE(xt::allclose(yield.nextYield(+3), r + 2.0));
            }
        }

        for (size_t j = 0; j < 40; ++j) {
            x -= 1.0;
            l -= 1.0;
            r -= 1.0;
            i -= 1;
            yield.setPosition(x);
            REQUIRE(xt::all(xt::equal(yield.currentIndex(), i)));

            REQUIRE(xt::allclose(yield.currentYieldLeft(), l));
            REQUIRE(xt::allclose(yield.currentYieldRight(), r));

            REQUIRE(xt::allclose(yield.currentYieldLeft(0), l));
            REQUIRE(xt::allclose(yield.currentYieldRight(0), r));

            REQUIRE(xt::allclose(yield.nextYield(-1), l));
            REQUIRE(xt::allclose(yield.nextYield(+1), r));

            if (xt::amin(yield.raw_idx())() > 0) {
                REQUIRE(xt::allclose(yield.currentYieldLeft(1), l - 1.0));
                REQUIRE(xt::allclose(yield.nextYield(-2), l - 1.0));
            }
            if (xt::amin(yield.raw_idx())() > 1) {
                REQUIRE(xt::allclose(yield.currentYieldLeft(2), l - 2.0));
                REQUIRE(xt::allclose(yield.nextYield(-3), l - 2.0));
            }

            if (xt::amax(yield.raw_idx())() < 30 - 2) {
                REQUIRE(xt::allclose(yield.currentYieldRight(1), r + 1.0));
                REQUIRE(xt::allclose(yield.nextYield(+2), r + 1.0));
            }
            if (xt::amax(yield.raw_idx())() < 30 - 3) {
                REQUIRE(xt::allclose(yield.currentYieldRight(2), r + 2.0));
                REQUIRE(xt::allclose(yield.nextYield(+3), r + 2.0));
            }
        }
    }

    SECTION("RedrawList::currentYield")
    {
        auto uniform = [=](std::vector<size_t> shape) { return xt::ones<double>(shape); };
        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};
        QPot::RedrawList yield(x, uniform, 30, 5, 2);
        xt::xtensor<double, 2> ret = {
            {2, 3, 4, 5, 6},
            {3, 4, 5, 6, 7},
            {4, 5, 6, 7, 8}};
        REQUIRE(xt::allclose(yield.currentYield(-2, 3), ret));
    }

    SECTION("RedrawList::yield - index")
    {
        auto uniform = [=](std::vector<size_t> shape) { return xt::ones<double>(shape); };
        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};
        QPot::RedrawList yield(x, uniform, 30, 5, 2);
        xt::xtensor<double, 1> ret = (0.0 - (14.5 - 5.5)) * xt::ones<double>({3});
        REQUIRE(xt::allclose(yield.yield(0), ret));
    }
}
