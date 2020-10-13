
#include <catch2/catch.hpp>
#include <QPot/Static.hpp>

TEST_CASE("QPot::Static", "Static.hpp")
{

SECTION("Index")
{
    xt::xtensor<double,1> y = xt::linspace<double>(0, 10, 11);

    QPot::Static yield(5.5, y);

    REQUIRE(yield.currentIndex() == 5);
    REQUIRE(yield.checkYieldBoundLeft());
    REQUIRE(yield.checkYieldBoundRight());
    REQUIRE(yield.currentYieldLeft() == Approx(5.0));
    REQUIRE(yield.currentYieldRight() == Approx(6.0));

    yield.setPosition(6.5);

    REQUIRE(yield.currentIndex() == 6);
    REQUIRE(yield.checkYieldBoundLeft());
    REQUIRE(yield.checkYieldBoundRight());
    REQUIRE(yield.currentYieldLeft() == Approx(6.0));
    REQUIRE(yield.currentYieldRight() == Approx(7.0));
}

}
