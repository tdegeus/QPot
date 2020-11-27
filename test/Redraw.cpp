
#include <catch2/catch.hpp>
#include <QPot/Redraw.hpp>

TEST_CASE("QPot::RedrawList", "Redraw.hpp")
{
    SECTION("Redraw")
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
            REQUIRE(xt::all(xt::equal(yield.currentYieldLeft(), l)));
            REQUIRE(xt::all(xt::equal(yield.currentYieldRight(), r)));
            REQUIRE(xt::all(xt::equal(yield.currentIndex(), i)));
        }

        for (size_t j = 0; j < 40; ++j) {
            x -= 1.0;
            l -= 1.0;
            r -= 1.0;
            i -= 1;
            yield.setPosition(x);
            REQUIRE(xt::all(xt::equal(yield.currentYieldLeft(), l)));
            REQUIRE(xt::all(xt::equal(yield.currentYieldRight(), r)));
            REQUIRE(xt::all(xt::equal(yield.currentIndex(), i)));
        }
    }

    SECTION("select")
    {
        auto uniform = [=](std::vector<size_t> shape) { return xt::ones<double>(shape); };
        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};
        QPot::RedrawList yield(x, uniform, 30, 5, 2);
        xt::xtensor<double, 2> ret = {
            {3, 4, 5, 6},
            {4, 5, 6, 7},
            {5, 6, 7, 8}};
        REQUIRE(xt::allclose(yield.yield(-2, 2), ret));
    }
}
