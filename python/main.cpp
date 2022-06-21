/**
Python API.

\file
\copyright Copyright 2017. Tom de Geus. All rights reserved.
\license This project is released under the MIT License.
*/

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#define FORCE_IMPORT_ARRAY
#include <xtensor-python/pyarray.hpp>
#include <xtensor-python/pytensor.hpp>

#define QPOT_ENABLE_WARNING_PYTHON
#include <QPot.h>

namespace py = pybind11;

/**
Overrides the `__name__` of a module.
Classes defined by pybind11 use the `__name__` of the module as of the time they are defined,
which affects the `__repr__` of the class type objects.
*/
class ScopedModuleNameOverride {
public:
    explicit ScopedModuleNameOverride(py::module m, std::string name) : module_(std::move(m))
    {
        original_name_ = module_.attr("__name__");
        module_.attr("__name__") = name;
    }
    ~ScopedModuleNameOverride()
    {
        module_.attr("__name__") = original_name_;
    }

private:
    py::module module_;
    py::object original_name_;
};

PYBIND11_MODULE(_QPot, m)
{
    // Ensure members to display as `QPot.X` (not `QPot._QPot.X`)
    ScopedModuleNameOverride name_override(m, "QPot");

    xt::import_numpy();

    m.doc() = "Library to keep track of a sequential potential energy landscape.";

    m.def("version", &QPot::version, "version");
    m.def("version_dependencies", &QPot::version_dependencies, "version_dependencies");

    m.def(
        "lower_bound",
        py::overload_cast<const xt::pyarray<double>&, const xt::pyarray<double>&>(
            &QPot::lower_bound<xt::pyarray<double>, xt::pyarray<double>, xt::pyarray<long>>),
        "Find column for each row.",
        py::arg("matrix"),
        py::arg("value"));

    m.def(
        "lower_bound",
        py::overload_cast<
            const xt::pyarray<double>&,
            const xt::pyarray<double>&,
            const xt::pyarray<long>&,
            size_t>(
            &QPot::lower_bound<xt::pyarray<double>, xt::pyarray<double>, xt::pyarray<long>>),
        "Find column for each row.",
        py::arg("matrix"),
        py::arg("value"),
        py::arg("index"),
        py::arg("proximity") = 10);

    m.def(
        "cumsum_chunk",
        &QPot::cumsum_chunk<xt::pyarray<double>, xt::pyarray<long>>,
        "Compute chunk of chunked cumsum",
        py::arg("cumsum"),
        py::arg("delta"),
        py::arg("shift"));

    py::module minplace = m.def_submodule("inplace", "In-place operations");

    minplace.def(
        "lower_bound",
        &QPot::inplace::lower_bound<xt::pyarray<double>, xt::pyarray<double>, xt::pyarray<long>>,
        "Find column for each row.",
        py::arg("matrix"),
        py::arg("value"),
        py::arg("index"),
        py::arg("proximity") = 10);

    minplace.def(
        "cumsum_chunk",
        &QPot::inplace::cumsum_chunk<xt::pyarray<double>, xt::pyarray<long>>,
        "Compute chunk of chunked cumsum",
        py::arg("cumsum"),
        py::arg("delta"),
        py::arg("shift"));
}
