#include <QPot/Chunked.hpp>
#include <prrng.h>
#include <xtensor/xtensor.hpp>
#include <xtensor/xrandom.hpp>
#include <xtensor/xadapt.hpp>
#include <xtensor/xsort.hpp>
#include <xtensor/xio.hpp>

int main()
{
    prrng::pcg32 generator;
    prrng::pcg32 ref = generator;

    xt::xtensor<size_t, 1> chunks = 1000 * xt::arange<size_t>(12);
    xt::xtensor<double, 1> xmin = xt::empty<double>({chunks.size()});
    xmin(xmin.size() - 1) = std::numeric_limits<double>::infinity();
    xt::xtensor<uint64_t, 1> state = xt::empty<uint64_t>({chunks.size() - 1});

    size_t ichunk = 0;

    state(ichunk) = generator.state<>();
    auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) - chunks(ichunk)});

    QPot::Chunked chunk(5.0, xt::cumsum(dy));
    xmin(ichunk) = chunk.ymin_chunk();

    size_t nbuffer = 20;

    // some sequence of positions
    xt::xtensor<double, 1> x = xt::random::rand<double>({100});
    x(0) += 1e-3;
    x = xt::cumsum(x);
    x /= x(x.size() - 1) / 5400.0;

    // characteristics of landscape (for checking)
    xt::xtensor<double, 1> yl = xt::empty<double>(x.shape());
    xt::xtensor<double, 1> yr = xt::empty<double>(x.shape());
    xt::xtensor<double, 1> yl_ = xt::empty<double>(x.shape());
    xt::xtensor<double, 1> yr_ = xt::empty<double>(x.shape());
    size_t i = 0;

    // loop though x, drawing yield positions if needed
    for (auto& xi : x) {
        chunk.set_x(xi);
        while (chunk.redraw() == +1) {
            ichunk++;
            state(ichunk) = generator.state<>();
            auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) - chunks(ichunk)});
            chunk.shift_dy(chunks(ichunk), dy, nbuffer);
            xmin(ichunk) = chunk.ymin_chunk();
        }
        if (!(chunk.boundcheck_left() && chunk.boundcheck_right())) {
            throw std::runtime_error("Out-of-bounds");
        }

        // store for checking
        yl(i) = chunk.yleft();
        yr(i) = chunk.yright();
        i++;
    }

    // reverse sequence of positions
    std::reverse(x.begin(), x.end());

    // roll back, re-rendering exactly the same random numbers
    for (auto& xi : x) {
        chunk.set_x(xi);
        while (chunk.redraw() == -1) {
            ichunk--;
            generator.restore(state(ichunk));
            auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) + nbuffer - chunks(ichunk)});
            chunk.shift_dy(chunks(ichunk), dy, nbuffer);
        }
        if (!(chunk.boundcheck_left() && chunk.boundcheck_right())) {
            throw std::runtime_error("Out-of-bounds");
        }

        // store for checking
        i--;
        yl_(i) = chunk.yleft();
        yr_(i) = chunk.yright();
    }

    if (!(xt::allclose(yl, yl_) && xt::allclose(yr, yr_))) {
        throw std::runtime_error("Reconstruction failure");
    }

    // evaluate in a random way
    xt::random::shuffle(x);

    for (i = 0; i < x.size(); ++i) {

        double xi = x(i);
        chunk.set_x(xi);

        ichunk = xt::argmax(xi < xmin)() - 1;

        generator.restore(state(ichunk));
        auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) + 1 - chunks(ichunk)});

        dy(0) = xmin(ichunk);

        chunk.set_y(chunks(ichunk), xt::cumsum(dy));

        yl_(i) = chunk.yleft();
        yr_(i) = chunk.yright();
    }

    auto idx = xt::argsort(x);
    x = xt::view(x, xt::keep(idx));
    yl_ = xt::view(yl_, xt::keep(idx));
    yr_ = xt::view(yr_, xt::keep(idx));

    if (!(xt::allclose(yl, yl_) && xt::allclose(yr, yr_))) {
        throw std::runtime_error("Reconstruction failure");
    }

    return 0;
}
