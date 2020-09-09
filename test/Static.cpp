
#include <catch2/catch.hpp>

#define EQ(a, b) REQUIRE_THAT((a), Catch::WithinAbs((b), 1.e-12));

#include <QPot/Static.hpp>

TEST_CASE("QPot::Static", "Static.hpp")
{

SECTION("Index")
{
    xt::xtensor<double,1> y = xt::linspace<double>(0, 10, 11);

    QPot::Static yield(5.5, y);

    REQUIRE(yield.currentIndex() == 5);
    EQ(yield.currentYieldLeft(), 5.0);
    EQ(yield.currentYieldRight(), 6.0);

    yield.setPosition(6.5);

    REQUIRE(yield.currentIndex() == 6);
    EQ(yield.currentYieldLeft(), 6.0);
    EQ(yield.currentYieldRight(), 7.0);
}

}
