# QPot

[![CI](https://github.com/tdegeus/QPot/workflows/CI/badge.svg)](https://github.com/tdegeus/QPot/actions)
[![Doxygen -> gh-pages](https://github.com/tdegeus/QPot/workflows/gh-pages/badge.svg)](https://tdegeus.github.io/QPot)
[![Conda Version](https://img.shields.io/conda/vn/conda-forge/qpot.svg)](https://anaconda.org/conda-forge/qpot)
[![Conda Version](https://img.shields.io/conda/vn/conda-forge/python-qpot.svg)](https://anaconda.org/conda-forge/python-qpot)

Library to keep track of a sequential potential energy landscape.

Documentation: https://tdegeus.github.io/QPot

## Contents

<!-- MarkdownTOC -->

- [Overview](#overview)
    - [Disclaimer](#disclaimer)
    - [More information](#more-information)
    - [Implementation](#implementation)
        - [C++ and Python](#c-and-python)
        - [Chunked.hpp](#chunkedhpp)
        - [Static.hpp](#statichpp)
        - [Redraw.hpp](#redrawhpp)
    - [Installation](#installation)
        - [C++ headers](#c-headers)
            - [Using conda](#using-conda)
            - [From source](#from-source)
        - [Python module](#python-module)
            - [Using conda](#using-conda-1)
            - [From source](#from-source-1)
    - [Compiling user-code](#compiling-user-code)
        - [Using CMake](#using-cmake)
            - [Example](#example)
            - [Targets](#targets)
            - [Optimisation](#optimisation)
        - [By hand](#by-hand)
        - [Using pkg-config](#using-pkg-config)
    - [Change-log](#change-log)
        - [v0.9.9](#v099)
        - [v0.9.8](#v098)
        - [v0.9.7](#v097)
        - [v0.9.6](#v096)
        - [v0.9.5](#v095)
        - [v0.9.4](#v094)
        - [v0.9.3](#v093)
        - [v0.9.2](#v092)
        - [v0.9.1](#v091)
        - [v0.9.0](#v090)
        - [v0.8.0](#v080)
        - [v0.7.0](#v070)
        - [v0.6.2](#v062)
        - [v0.6.1](#v061)
        - [v0.6.0](#v060)
        - [v0.5.0](#v050)
        - [v0.4.0](#v040)
        - [v0.3.0](#v030)
        - [v0.2.0](#v020)

<!-- /MarkdownTOC -->

## Overview

### Disclaimer

This library is free to use under the
[MIT license](https://github.com/tdegeus/QPot/blob/master/LICENSE).
Any additions are very much appreciated, in terms of suggested functionality, code,
documentation, testimonials, word-of-mouth advertisement, etc.
Bug reports or feature requests can be filed on
[GitHub](https://github.com/tdegeus/QPot).
As always, the code comes with no guarantee.
None of the developers can be held responsible for possible mistakes.

Download:
[.zip file](https://github.com/tdegeus/QPot/zipball/master) |
[.tar.gz file](https://github.com/tdegeus/QPot/tarball/master).

(c - [MIT](https://github.com/tdegeus/QPot/blob/master/LICENSE))
T.W.J. de Geus (Tom) | tom@geus.me | www.geus.me |
[github.com/tdegeus/QPot](https://github.com/tdegeus/QPot)

### More information

*   The documentation of the code.
*   The code itself.
*   The unit tests, under [test](./test).
*   The examples, under [examples](./examples).

### Implementation

#### C++ and Python

The code is a C++ header-only library (see [installation notes](#c-headers)),
but a Python module is also provided (see [installation notes](#python-module)).
The interfaces are identical except:

+   All *xtensor* objects (`xt::xtensor<...>`) are *NumPy* arrays in Python.
+   All `::` in C++ are `.` in Python.

#### Chunked.hpp

Storage of a chunk of the series of yield positions,
including support to move along the full series by providing sequential chunks upon request.

```cpp
#include <QPot/Chunked.hpp>

int main()
{
    xt::xtensor<double,1> y = xt::linspace<double>(-1, 10, 12);

    QPot::Chunked yield(0.0, y);

    std::cout << yield.i() << std::endl;
    std::cout << yield.yleft() << std::endl;
    std::cout << yield.yright() << std::endl;

    yield.set_x(5.5);

    return 0;
}
```

See QPot::Chunked for more information.
Furthermore, please find [this example](./examples/frozen_sequence/main.cpp).

#### Static.hpp

Static list of yield points.

```cpp
#include <QPot/Static.hpp>

int main()
{
    xt::xtensor<double,1> y = xt::linspace<double>(-1, 10, 12);

    QPot::Static yield(0.0, y);

    std::cout << yield.currentIndex() << std::endl;
    std::cout << yield.currentYieldLeft() << std::endl;
    std::cout << yield.currentYieldRight() << std::endl;

    yield.setPosition(5.5);

    std::cout << yield.currentIndex() << std::endl;
    std::cout << yield.currentYieldLeft() << std::endl;
    std::cout << yield.currentYieldRight() << std::endl;

    return 0;
}
```

See QPot::Static for more information.

#### Redraw.hpp

Dynamically redraw yield points.

```cpp
#include <QPot/Redraw.hpp>

int main()
{
    auto uniform = [=](std::array<size_t, 2> shape) {
        return xt::ones<double>(shape); };

    size_t N = 10;
    xt::xtensor<double,1> x = xt::zeros<double>({N});

    QPot::RedrawList yield(x, uniform);

    std::cout << yield.currentIndex() << std::endl;
    std::cout << yield.currentYieldLeft() << std::endl;
    std::cout << yield.currentYieldRight() << std::endl;

    x.fill(5.5)
    yield.setPosition(x);

    std::cout << yield.currentIndex() << std::endl;
    std::cout << yield.currentYieldLeft() << std::endl;
    std::cout << yield.currentYieldRight() << std::endl;

    return 0;
}
```

>   From Python one can use:
>
>   ```python
>   def uniform(shape):
>       return np.ones(shape)
>
>   x = np.random.rand([100])
>   y = QPot.RedrawList(x, uniform);
>   ```

See QPot::RedrawList for more information.

### Installation

#### C++ headers

##### Using conda

```bash
conda install -c conda-forge qpot
```

##### From source

```bash
# Download QPot
git checkout https://github.com/tdegeus/QPot.git
cd QPot

# Install headers, CMake and pkg-config support
cmake .
make install
```

#### Python module

##### Using conda

```bash
conda install -c conda-forge python-qpot
```

Note that *xsimd* and hardware optimisation are **not enabled**.
To enable them you have to compile on your system, as is discussed next.

##### From source

>   You need *xtensor*, *xtensor-python* and optionally *xsimd* as prerequisites.
>   In addition *scikit-build* is needed to control the build from Python.
>   The easiest is to use *conda* to get the prerequisites:
>
>   ```bash
>   conda install -c conda-forge xtensor-python
>   conda install -c conda-forge xsimd
>   conda install -c conda-forge scikit-build
>   ```
>
>   If you then compile and install with the same environment
>   you should be good to go.
>   Otherwise, a bit of manual labour might be needed to
>   treat the dependencies.

```bash
# Download QPot
git checkout https://github.com/tdegeus/QPot.git
cd QPot

# Compile and install the Python module
# (-vv can be omitted as is controls just the verbosity)
python setup.py install --build-type Release -vv

# OR, Compile and install the Python module with hardware optimisation
# (with scikit-build CMake options can just be added as command-line arguments)
python setup.py install --build-type Release -DUSE_SIMDD=1 -vv
```

### Compiling user-code

#### Using CMake

##### Example

Using *QPot* your `CMakeLists.txt` can be as follows

```cmake
cmake_minimum_required(VERSION 3.1)
project(example)
find_package(QPot REQUIRED)
add_executable(example example.cpp)
target_link_libraries(example PRIVATE QPot)
```

##### Targets

The following targets are available:

*   `QPot`
    Includes *QPot* and the *xtensor* dependency.

*   `QPot::assert`
    Enables assertions by defining `QPOT_ENABLE_ASSERT`.

*   `QPot::debug`
    Enables all assertions by defining
    `QPOT_ENABLE_ASSERT` and `XTENSOR_ENABLE_ASSERT`.

*   `QPot::compiler_warings`
    Enables compiler warnings (generic).

##### Optimisation

It is advised to think about compiler optimisation and enabling *xsimd*.
Using *CMake* this can be done using the `xtensor::optimize` and `xtensor::use_xsimd` targets.
The above example then becomes:

```cmake
cmake_minimum_required(VERSION 3.1)
project(example)
find_package(QPot REQUIRED)
find_package(xtensor REQUIRED)
find_package(xsimd REQUIRED)
add_executable(example example.cpp)
target_link_libraries(example PRIVATE
    QPot
    xtensor::optimize
    xtensor::use_xsimd)
```

See the [documentation of xtensor](https://xtensor.readthedocs.io/en/latest/) concerning optimisation.

#### By hand

Presuming that the compiler is `c++`, compile using:

```
c++ -I/path/to/QPot/include ...
```

Note that you have to take care of the *xtensor* dependency, the C++ version, optimisation,
enabling *xsimd*, ...

#### Using pkg-config

Presuming that the compiler is `c++`, compile using:

```
c++ `pkg-config --cflags QPot` ...
```

Note that you have to take care of the *xtensor* dependency, the C++ version, optimization,
enabling *xsimd*, ...

### Change-log

#### v0.9.9

*   [CMake] Minor style updates (#37)

#### v0.9.8

*   [Python] Build with scikit-build (#36)

#### v0.9.7

*   [docs] Minor readme updates
*   [CMake] Minor style updates (#35)
*   [CMake] Improving handling options
*   [Python] Switching to xtensor-python : avoids copies (#33)


#### v0.9.6

*   [setup.py] Adding missing import
*   [CI] Changing dummy version
*   Adding global CMake options (+ updating CI)
*   Adding missing include
*   Avoiding *setuptools_scm* dependency if `SETUPTOOLS_SCM_PRETEND_VERSION` is defined
*   Building docs on release

#### v0.9.5

*   Minor bugfix bounds-check: including assertion of lock in criterion
*   CMake updates.

#### v0.9.4

*   Adding convenience function `Chunked::size`.

#### v0.9.3

*   Adding convenience function `i_chunk`.

#### v0.9.2

*   Updating docs, readme, and example.
*   `QPot::Chunked::redraw`: Allow trial.
*   Applying latest pcg32 features.
*   Adding test for `Chunked::ymin_chunk`.

#### v0.9.1

*   Minor CMake updates.
*   Integrating Python API in CMake.
*   Marking `Chunked::i()` const (#24).

#### v0.9.0

*   Adding: Chunked storage (#23)
*   Fixing version in doxygen docs
*   [CI] Minor style update
*   Updating doxygen-awesome

#### v0.8.0

*   Simplifying CMake
*   Minor documentation updates
*   API change: "yield()" -> "yieldPosition()" to avoid broken Python API

#### v0.7.0

*   Various documentation updates.
*   Adding (optional) HDF5 test to test platform independence of sequence restore).
*   Allowing restoring a sequence with less data.
*   Allow prompt if a redraw was triggered on the last position update.

#### v0.6.2

*   Python API: relaxing dependencies.
*   RedrawList: Adding extra assertion on position.

#### v0.6.1

*   Bugfix: making sure that "m_idx" is computed correctly after a redraw

#### v0.6.0

*   Adding QPot::random interface to simplify RedrawList and to make reconstruction in Python possible (see test).
*   [docs] Dark background.

#### v0.5.0

*   Allow reconstruction of random RedrawList (see test-case).
*   Adding documentation using doxygen.
*   Switching to setuptools_scm for versioning.
*   Minor style updates.

#### v0.4.0

*   Adding `nextYield`.

#### v0.3.0

*   Switching to GitHub CI.
*   Adding `yield` to return the entire landscape or a cross-section.
*   Adding offset overloads to `currentYieldLeft` and `currentYieldRight`.
*   Adding `currentYield` to get the landscape around the current position.

#### v0.2.0

*   Adding "checkYieldBoundLeft" and "checkYieldBoundRight" to "Static".
*   Using Catch2 to test for floats.
