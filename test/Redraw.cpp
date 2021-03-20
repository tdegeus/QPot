
#include <catch2/catch.hpp>
#include <QPot/Redraw.hpp>
#include <QPot/random.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xrandom.hpp>

TEST_CASE("QPot::RedrawList", "Redraw.hpp")
{
    SECTION("Basic behaviour")
    {
        xt::xtensor<double, 1> x = {-1.0, 0.0, 1.0};
        xt::xtensor<double, 1> l = {-1.5, -0.5, 0.5};
        xt::xtensor<double, 1> r = l + 1.0;
        xt::xtensor<long, 1> i = {13, 14, 15};

        auto uniform = QPot::random::UniformList();

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
        auto uniform = QPot::random::UniformList();
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
        auto uniform = QPot::random::UniformList();
        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};
        QPot::RedrawList yield(x, uniform, 30, 5, 2);
        xt::xtensor<double, 1> ret = (0.0 - (14.5 - 5.5)) * xt::ones<double>({3});
        REQUIRE(xt::allclose(yield.yield(0), ret));
    }

    SECTION("RedrawList - reproduce sequence")
    {
        auto seed = time(NULL);
        auto rand = QPot::random::RandList();

        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};

        QPot::random::seed(seed);
        QPot::RedrawList yield(x, rand, 30, 5, 2);
        std::vector<bool> redraw;

        for (size_t i = 0; i < 50; ++i) {
            redraw.push_back(yield.setPosition(xt::eval((double)i * x)));
        }

        auto pos = yield.raw_pos();

        QPot::random::seed(seed);
        QPot::RedrawList other(x, rand, 30, 5, 2);
        for (size_t i = 0; i < redraw.size(); ++i) {
            if (redraw[i]) {
                other.setPosition(xt::eval((double)i * x));
            }
        }

        REQUIRE(xt::allclose(pos, other.raw_pos()));
    }
}
