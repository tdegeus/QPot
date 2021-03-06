
#include <catch2/catch.hpp>
#include <QPot/Static.hpp>

TEST_CASE("QPot::Static", "Static.hpp")
{
    SECTION("Basic behaviour")
    {
        xt::xtensor<double, 1> y = xt::linspace<double>(0, 10, 11);

        QPot::Static yield(5.5, y);

        REQUIRE(yield.currentIndex() == 5);

        REQUIRE(yield.checkYieldBoundLeft());
        REQUIRE(yield.checkYieldBoundRight());

        REQUIRE(xt::allclose(yield.currentYieldLeft(), 5.0));
        REQUIRE(xt::allclose(yield.currentYieldRight(), 6.0));

        REQUIRE(xt::allclose(yield.currentYieldLeft(0), yield.currentYieldLeft()));
        REQUIRE(xt::allclose(yield.currentYieldLeft(1), 4.0));
        REQUIRE(xt::allclose(yield.currentYieldLeft(2), 3.0));

        REQUIRE(xt::allclose(yield.currentYieldRight(0), yield.currentYieldRight()));
        REQUIRE(xt::allclose(yield.currentYieldRight(1), 7.0));
        REQUIRE(xt::allclose(yield.currentYieldRight(2), 8.0));

        REQUIRE(xt::allclose(yield.currentYieldRight(0), yield.currentYieldRight()));
        REQUIRE(xt::allclose(yield.currentYieldRight(1), 7.0));
        REQUIRE(xt::allclose(yield.currentYieldRight(2), 8.0));

        REQUIRE(xt::allclose(yield.nextYield(-1), yield.currentYieldLeft()));
        REQUIRE(xt::allclose(yield.nextYield(-2), 4.0));
        REQUIRE(xt::allclose(yield.nextYield(-3), 3.0));

        REQUIRE(xt::allclose(yield.nextYield(+1), yield.currentYieldRight()));
        REQUIRE(xt::allclose(yield.nextYield(+2), 7.0));
        REQUIRE(xt::allclose(yield.nextYield(+3), 8.0));

        yield.setPosition(6.5);

        REQUIRE(yield.currentIndex() == 6);

        REQUIRE(yield.checkYieldBoundLeft());
        REQUIRE(yield.checkYieldBoundRight());

        REQUIRE(xt::allclose(yield.currentYieldLeft(), 6.0));
        REQUIRE(xt::allclose(yield.currentYieldRight(), 7.0));

        REQUIRE(xt::allclose(yield.currentYieldLeft(0), yield.currentYieldLeft()));
        REQUIRE(xt::allclose(yield.currentYieldLeft(1), 5.0));
        REQUIRE(xt::allclose(yield.currentYieldLeft(2), 4.0));

        REQUIRE(xt::allclose(yield.currentYieldRight(0), yield.currentYieldRight()));
        REQUIRE(xt::allclose(yield.currentYieldRight(1), 8.0));
        REQUIRE(xt::allclose(yield.currentYieldRight(2), 9.0));

        REQUIRE(xt::allclose(yield.nextYield(-1), yield.currentYieldLeft()));
        REQUIRE(xt::allclose(yield.nextYield(-2), 5.0));
        REQUIRE(xt::allclose(yield.nextYield(-3), 4.0));

        REQUIRE(xt::allclose(yield.nextYield(+1), yield.currentYieldRight()));
        REQUIRE(xt::allclose(yield.nextYield(+2), 8.0));
        REQUIRE(xt::allclose(yield.nextYield(+3), 9.0));
    }

    SECTION("Static::currentYield")
    {
        xt::xtensor<double, 1> y = xt::linspace<double>(0, 10, 11);
        QPot::Static yield(5.5, y);
        xt::xtensor<double, 1> ret = {3, 4, 5, 6, 7};
        REQUIRE(xt::allclose(yield.currentYield(-2, 3), ret));
    }

    SECTION("Static::yield - index")
    {
        xt::xtensor<double, 1> y = xt::linspace<double>(0, 10, 11);
        QPot::Static yield(5.5, y);
        REQUIRE(xt::allclose(yield.yieldPosition(0), 0.0));
        REQUIRE(xt::allclose(yield.yieldPosition(1), 1.0));
        REQUIRE(xt::allclose(yield.yieldPosition(2), 2.0));
        REQUIRE(xt::allclose(yield.yieldPosition(3), 3.0));
        REQUIRE(xt::allclose(yield.yieldPosition(4), 4.0));
        REQUIRE(xt::allclose(yield.yieldPosition(5), 5.0));
        REQUIRE(xt::allclose(yield.yieldPosition(6), 6.0));
        REQUIRE(xt::allclose(yield.yieldPosition(7), 7.0));
        REQUIRE(xt::allclose(yield.yieldPosition(8), 8.0));
        REQUIRE(xt::allclose(yield.yieldPosition(9), 9.0));
        REQUIRE(xt::allclose(yield.yieldPosition(10), 10.0));
    }
}
