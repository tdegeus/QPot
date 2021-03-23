
#include <catch2/catch.hpp>
#include <QPot/Redraw.hpp>
#include <QPot/random.hpp>
#include <xtensor/xio.hpp>
#include <xtensor/xrandom.hpp>
#include <highfive/H5Easy.hpp>

TEST_CASE("QPot::RedrawList", "Redraw.hpp")
{
    SECTION("Basic behaviour (includes tests for convenience functions)")
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

    SECTION("Slice around currentYield")
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

    SECTION("Slice of yield")
    {
        auto uniform = QPot::random::UniformList();
        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};
        QPot::RedrawList yield(x, uniform, 30, 5, 2);
        xt::xtensor<double, 1> ret = (0.0 - (14.5 - 5.5)) * xt::ones<double>({3});
        REQUIRE(xt::allclose(yield.yield(0), ret));
    }

    SECTION("Large step size")
    {
        xt::xtensor<double, 1> x = {0.0, 0.0, 0.0};
        xt::xtensor<double, 1> l = {-0.5, -0.5, -0.5};
        xt::xtensor<double, 1> r = l + 1.0;
        xt::xtensor<double, 1> dx = {20.0, 2.0, 1.0};

        auto uniform = QPot::random::UniformList();
        QPot::RedrawList yield(x, uniform, 30, 5, 2);

        REQUIRE(xt::allclose(yield.currentYieldLeft(), l));
        REQUIRE(xt::allclose(yield.currentYieldRight(), r));

        yield.setPosition(x + dx);

        REQUIRE(xt::allclose(yield.currentYieldLeft(), l + dx));
        REQUIRE(xt::allclose(yield.currentYieldRight(), r + dx));
    }

    SECTION("Reproduce redrawn sequence: restore positions that triggered a redraw")
    {
        auto seed = time(NULL);
        auto rand = QPot::random::RandList();

        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};

        QPot::random::seed(seed);
        QPot::RedrawList self(x, rand, 30, 5, 2);
        size_t n = 50;

        // ``true`` is redraw was triggered
        std::vector<bool> redraw(n);

        for (size_t i = 0; i < n; ++i) {
            bool r = self.setPosition(xt::eval((double)i * x));
            redraw[i] = r;
        }

        auto pos = self.raw_pos();
        auto yl = self.currentYieldLeft();
        auto yr = self.currentYieldRight();
        auto yi = self.currentIndex();

        QPot::random::seed(seed);
        QPot::RedrawList other(x, rand, 30, 5, 2);

        for (size_t i = 0; i < redraw.size(); ++i) {
            if (redraw[i]) {
                other.setPosition(xt::eval((double)i * x));
            }
        }

        other.setPosition(xt::eval(double(n - 1) * x));

        REQUIRE(xt::allclose(pos, other.raw_pos()));
        REQUIRE(xt::allclose(yl, other.currentYieldLeft()));
        REQUIRE(xt::allclose(yr, other.currentYieldRight()));
        REQUIRE(xt::allclose(yi, other.currentIndex()));
    }

    SECTION("Reproduce redrawn sequence: restore 'iredraw' of each step")
    {
        auto seed = time(NULL);
        auto rand = QPot::random::RandList();

        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};

        QPot::random::seed(seed);
        QPot::RedrawList self(x, rand, 30, 5, 2);
        size_t n = 50;

        std::vector<xt::xtensor<int, 1>> iredraw(n);

        for (size_t i = 0; i < n; ++i) {
            self.setPosition(xt::eval((double)i * x));
            iredraw[i] = self.currentRedraw();
        }

        auto pos = self.raw_pos();
        auto yl = self.currentYieldLeft();
        auto yr = self.currentYieldRight();
        auto yi = self.currentIndex();

        QPot::random::seed(seed);
        QPot::RedrawList other(x, rand, 30, 5, 2);

        for (auto& i : iredraw) {
            other.redraw(i);
        }

        other.setPosition(xt::eval(double(n - 1) * x));

        REQUIRE(xt::allclose(pos, other.raw_pos()));
        REQUIRE(xt::allclose(yl, other.currentYieldLeft()));
        REQUIRE(xt::allclose(yr, other.currentYieldRight()));
        REQUIRE(xt::allclose(yi, other.currentIndex()));
    }

    SECTION("Reproduce redrawn sequence: restore 'iredraw' only when there was a redraw")
    {
        auto seed = time(NULL);
        auto rand = QPot::random::RandList();

        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};

        QPot::random::seed(seed);
        QPot::RedrawList self(x, rand, 30, 5, 2);
        size_t n = 50;

        std::vector<xt::xtensor<int, 1>> iredraw;

        for (size_t i = 0; i < n; ++i) {
            bool r = self.setPosition(xt::eval((double)i * x));
            if (r) {
                iredraw.push_back(self.currentRedraw());
            }
        }

        auto pos = self.raw_pos();
        auto yl = self.currentYieldLeft();
        auto yr = self.currentYieldRight();
        auto yi = self.currentIndex();

        QPot::random::seed(seed);
        QPot::RedrawList other(x, rand, 30, 5, 2);

        for (auto& i : iredraw) {
            other.redraw(i);
        }

        other.setPosition(xt::eval(double(n - 1) * x));

        REQUIRE(xt::allclose(pos, other.raw_pos()));
        REQUIRE(xt::allclose(yl, other.currentYieldLeft()));
        REQUIRE(xt::allclose(yr, other.currentYieldRight()));
        REQUIRE(xt::allclose(yi, other.currentIndex()));
    }

    SECTION("Reproduce redrawn sequence: minimal data storage (only lists of redrawn particles)")
    {
        // WARNING: one has to call "redrawRight" before "redrawLeft" to restore the sequence

        auto seed = time(NULL);
        auto rand = QPot::random::RandList();

        xt::xtensor<double, 1> x = {4.5, 5.5, 6.5};

        QPot::random::seed(seed);
        QPot::RedrawList self(x, rand, 30, 5, 2);
        size_t n = 50;

        std::vector<int> direction;
        std::vector<xt::xtensor<size_t, 1>> index;

        for (size_t i = 0; i < n; ++i) {
            if (self.setPosition(xt::eval((double)i * x))) {
                auto iredraw = self.currentRedraw();
                xt::xtensor<size_t, 1> r = xt::flatten_indices(xt::argwhere(iredraw > 0));
                index.push_back(r);
                direction.push_back(+1);
                xt::xtensor<size_t, 1> l = xt::flatten_indices(xt::argwhere(iredraw < 0));
                index.push_back(l);
                direction.push_back(-1);
            }
        }

        auto pos = self.raw_pos();
        auto yl = self.currentYieldLeft();
        auto yr = self.currentYieldRight();
        auto yi = self.currentIndex();

        QPot::random::seed(seed);
        QPot::RedrawList other(x, rand, 30, 5, 2);

        for (size_t i = 0; i < direction.size(); ++i) {
            if (direction[i] > 0) {
                other.redrawRight(index[i]);
            }
            else {
                other.redrawLeft(index[i]);
            }
        }

        other.setPosition(xt::eval(double(n - 1) * x));

        REQUIRE(xt::allclose(pos, other.raw_pos()));
        REQUIRE(xt::allclose(yl, other.currentYieldLeft()));
        REQUIRE(xt::allclose(yr, other.currentYieldRight()));
        REQUIRE(xt::allclose(yi, other.currentIndex()));
    }

    SECTION("Check platform independence")
    {
        H5Easy::File data("Redraw_reconstruct-data.h5", H5Easy::File::ReadOnly);

        auto rand = QPot::random::RandList();
        QPot::random::seed(H5Easy::load<size_t>(data, "/seed"));

        auto n = H5Easy::load<size_t>(data, "/n");
        auto redraw = H5Easy::load<xt::xtensor<int, 2>>(data, "/redraw");
        auto x = H5Easy::load<xt::xtensor<double, 1>>(data, "/x");

        QPot::RedrawList self(x, rand,
            H5Easy::load<size_t>(data, "/ntotal"),
            H5Easy::load<size_t>(data, "/nbuffer"),
            H5Easy::load<size_t>(data, "/noffset"));

        for (size_t i = 0; i < n; ++i) {
            xt::xtensor<int, 1> r = xt::view(redraw, xt::all(), i);
            self.redraw(r);
        }

        self.setPosition((double)(n - 1) * x);

        REQUIRE(xt::allclose(self.raw_pos(), H5Easy::load<xt::xtensor<double, 2>>(data, "/raw_pos")));
        REQUIRE(xt::allclose(self.currentYieldLeft(), H5Easy::load<xt::xtensor<double, 1>>(data, "/currentYieldLeft")));
        REQUIRE(xt::allclose(self.currentYieldRight(), H5Easy::load<xt::xtensor<double, 1>>(data, "/currentYieldRight")));
        REQUIRE(xt::allclose(self.currentIndex(), H5Easy::load<xt::xtensor<long, 1>>(data, "/currentIndex")));
    }
}
