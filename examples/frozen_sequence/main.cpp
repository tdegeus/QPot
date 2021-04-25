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
    xt::xtensor<double, 1> xmin = xt::empty<double>({chunks.size() - 1});
    xt::xtensor<double, 1> xmax = xt::empty<double>({chunks.size() - 1});
    xt::xtensor<uint64_t, 1> state = xt::empty<uint64_t>({chunks.size() - 1});
    size_t ichunk = 0;

    state(ichunk) = generator.state<>();
    auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) - chunks(ichunk)});

    QPot::Chunked chunk(5.0, xt::cumsum(dy));
    xmin(ichunk) = chunk.ymin();
    xmax(ichunk) = chunk.ymax();

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
            chunk.shift_dy(chunks(ichunk), dy, 1);
            xmin(ichunk) = chunk.ymin();
            xmax(ichunk) = chunk.ymax();
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
            auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) + 1 - chunks(ichunk)});
            chunk.shift_dy(chunks(ichunk), dy, 1);
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

        ichunk = xt::argmax(xi < xmax)();

        generator.restore(state(ichunk));
        if (ichunk > 0) {
            generator.advance(-1); // distance of the buffer
        }
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


    // for (size_t i = 99; i < 100; ++i) {
    //     chunk.set_x(static_cast<double>(i) * 55);
    //     while (chunk.redraw() == +1) {
    //         ichunk++;
    //         state(ichunk) = generator.state<>();
    //         auto dy = generator.random<xt::xtensor<double, 1>>({chunks(ichunk + 1) - chunks(ichunk)});
    //         auto istart = chunk.istop();
    //         chunk.shift_dy(istart, dy, 1);
    //     }
    // }



    // auto xy0 = landscape.chunk_xyield();

    // landscape.chuck_rshift_dxyield(generator.random<xt::xtensor<double, 1>>({100}));
    // auto xy1 = landscape.chunk_xyield();

    // landscape.chuck_rshift_dxyield(generator.random<xt::xtensor<double, 1>>({100}));
    // auto xy2 = landscape.chunk_xyield();

    // landscape.chuck_rshift_dxyield(generator.random<xt::xtensor<double, 1>>({100}));
    // auto xy3 = landscape.chunk_xyield();

    // generator.advance(-100);
    // landscape.chunk_lshift_dxyield(generator.random<xt::xtensor<double, 1>>({100}));
    // if (!xt::allclose(xy2, landscape.chunk_xyield())) {
    //     throw std::runtime_error("Disorder not frozen");
    // }

    // generator.advance(-100);
    // landscape.chunk_lshift_dxyield(generator.random<xt::xtensor<double, 1>>({100}));
    // if (!xt::allclose(xy1, landscape.chunk_xyield())) {
    //     throw std::runtime_error("Disorder not frozen");
    // }

    // generator.advance(-100);
    // landscape.chunk_lshift_dxyield(generator.random<xt::xtensor<double, 1>>({100}));
    // if (!xt::allclose(xy0, landscape.chunk_xyield())) {
    //     throw std::runtime_error("Disorder not frozen");
    // }

}
