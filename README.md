# QPot

[![CI](https://github.com/tdegeus/QPot/workflows/CI/badge.svg)](https://github.com/tdegeus/QPot/actions)
[![Doxygen -> gh-pages](https://github.com/tdegeus/QPot/workflows/gh-pages/badge.svg)](https://tdegeus.github.io/QPot)
[![Conda Version](https://img.shields.io/conda/vn/conda-forge/qpot.svg)](https://anaconda.org/conda-forge/qpot)
[![Conda Version](https://img.shields.io/conda/vn/conda-forge/python-qpot.svg)](https://anaconda.org/conda-forge/python-qpot)

Library to keep track of a sequential potential energy landscape.

Documentation:

*   This readme
*   [Doxygen documentation](https://tdegeus.github.io/QPot)
*   [The tests (in C++ and Python)](test/)

# Disclaimer

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

# Implementation

## C++ and Python

The code is a C++ header-only library (see [installation notes](#c-headers)), 
but a Python module is also provided (see [installation notes](#python-module)).
The interfaces are identical except:

+   All *xtensor* objects (`xt::xtensor<...>`) are *NumPy* arrays in Python. 
+   All `::` in C++ are `.` in Python.

## Static.hpp

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

To get more information about the yield positions:

*   `currentYieldRight(offset)`:
    -   `currentYieldRight(0) == currentYieldRight()`.
    -   `currentYieldRight(1)` gives the following yield positions to the right, etc.
*   `currentYieldLeft(offset)`:
    -   `currentYieldLeft(0) == currentYieldLeft()`.
    -   `currentYieldLeft(1)` gives the following yield positions to the left, etc.
*   `nextYield(offset)`:
    -   `nextYield(1) == currentYieldRight() == currentYieldRight(0)`  
    -   `nextYield(2) == currentYieldRight(1)`
    -   etc.  
    -   `nextYield(-1) == currentYieldLeft() == currentYieldLeft(0)`  
    -   `nextYield(-2) == currentYieldLeft(1)`
    -   etc.

## Redraw.hpp

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

To get more information about the yield positions:

*   `currentYieldRight(offset)`:
    -   `currentYieldRight(0) == currentYieldRight()`.
    -   `currentYieldRight(1)` gives the following yield positions to the right, etc.
*   `currentYieldLeft(offset)`:
    -   `currentYieldLeft(0) == currentYieldLeft()`.
    -   `currentYieldLeft(1)` gives the following yield positions to the left, etc.
*   `nextYield(offset)`:
    -   `nextYield(1) == currentYieldRight() == currentYieldRight(0)`  
    -   `nextYield(2) == currentYieldRight(1)`
    -   etc.  
    -   `nextYield(-1) == currentYieldLeft() == currentYieldLeft(0)`  
    -   `nextYield(-2) == currentYieldLeft(1)`
    -   etc.

# Installation

## C++ headers

### Using conda

```bash
conda install -c conda-forge qpot
```

### From source

```bash
# Download QPot
git checkout https://github.com/tdegeus/QPot.git
cd QPot

# Install headers, CMake and pkg-config support
cmake .
make install
```

## Python module

### Using conda

```bash
conda install -c conda-forge python-qpot
```

Note that *xsimd* and hardware optimisations are **not enabled**. 
To enable them you have to compile on your system, as is discussed next.

### From source

>   You need *xtensor*, *pyxtensor* and optionally *xsimd* as prerequisites. 
>   Additionally, Python needs to know how to find them. 
>   The easiest is to use *conda* to get the prerequisites:
> 
>   ```bash
>   conda install -c conda-forge pyxtensor
>   conda install -c conda-forge xsimd
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
python setup.py build
python setup.py install
# OR you can use one command (but with less readable output)
python -m pip install .
```

# Compiling

## Using CMake

### Example

Using *QPot* your `CMakeLists.txt` can be as follows

```cmake
cmake_minimum_required(VERSION 3.1)
project(example)
find_package(QPot REQUIRED)
add_executable(example example.cpp)
target_link_libraries(example PRIVATE QPot)
```

### Targets

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

### Optimisation

It is advised to think about compiler optimization and enabling *xsimd*.
Using *CMake* this can be done using the `xtensor::optimize` and `xtensor::use_xsimd` targets.
The above example then becomes:

```cmake
cmake_minimum_required(VERSION 3.1)
project(example)
find_package(QPot REQUIRED)
add_executable(example example.cpp)
target_link_libraries(example PRIVATE 
    QPot 
    xtensor::optimize 
    xtensor::use_xsimd)
```

See the [documentation of xtensor](https://xtensor.readthedocs.io/en/latest/) concerning optimization.

## By hand

Presuming that the compiler is `c++`, compile using:

```
c++ -I/path/to/QPot/include ...
```

Note that you have to take care of the *xtensor* dependency, the C++ version, optimization, 
enabling *xsimd*, ...

## Using pkg-config

Presuming that the compiler is `c++`, compile using:

```
c++ `pkg-config --cflags QPot` ...
```

Note that you have to take care of the *xtensor* dependency, the C++ version, optimization, 
enabling *xsimd*, ...

# Change-log

## v0.7.0

*   Various documentation updates.
*   Adding (optional) HDF5 test to test platform independence of sequence restore).
*   Allowing restoring a sequence with less data. 
*   Allow prompt if a redraw was triggered on the last position update.

## v0.6.2

*   Python API: relaxing dependencies.
*   RedrawList: Adding extra assertion on position.

## v0.6.1

*   Bugfix: making sure that "m_idx" is computed correctly after a redraw

## v0.6.0

*   Adding QPot::random interface to simplify RedrawList and to make reconstruction in Python possible (see test).
*   [docs] Dark background.

## v0.5.0

*   Allow reconstruction of random RedrawList (see test-case).
*   Adding documentation using doxygen.
*   Switching to setuptools_scm for versioning.
*   Minor style updates.

## v0.4.0

*   Adding `nextYield`.

## v0.3.0

*   Switching to GitHub CI.
*   Adding `yield` to return the entire landscape or a cross-section.
*   Adding offset overloads to `currentYieldLeft` and `currentYieldRight`.
*   Adding `currentYield` to get the landscape around the current position.

## v0.2.0

*   Adding "checkYieldBoundLeft" and "checkYieldBoundRight" to "Static".
*   Using Catch2 to test for floats.
