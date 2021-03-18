/*

(c - MIT) T.W.J. de Geus (Tom) | www.geus.me | github.com/tdegeus/QPot

*/

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pyxtensor/pyxtensor.hpp>

// Enable basic assertions on matrix shape
// (doesn't cost a lot of time, but avoids segmentation faults)
#define QPOT_ENABLE_ASSERT

#include <QPot/Static.hpp>
#include <QPot/Redraw.hpp>
#include <QPot/version.hpp>

namespace py = pybind11;

PYBIND11_MODULE(QPot, m)
{

m.doc() = "Library to keep track of a sequential potential energy landscape.";

m.def("version", &QPot::version, "version");
m.def("version_dependencies", &QPot::version_dependencies, "version_dependencies");

py::class_<QPot::Static>(m, "Static")

    .def(
        py::init<double, xt::xtensor<double, 1>>(),
        "Static yield sequence.",
        py::arg("x"),
        py::arg("yield"))

    .def(
        "yield",
        py::overload_cast<>(&QPot::Static::yield, py::const_),
        "Yield positions")

    .def(
        "yield",
        py::overload_cast<size_t>(&QPot::Static::yield, py::const_),
        "Specific yield position",
        py::arg("i"))

    .def(
        "yield",
        py::overload_cast<size_t, size_t>(&QPot::Static::yield, py::const_),
        "Range of yield position",
        py::arg("start"),
        py::arg("stop"))

    .def("setProximity", &QPot::Static::setProximity,
        "Customise proximity search",
        py::arg("proximity"))

    .def("setPosition", &QPot::Static::setPosition,
        "Update current position",
        py::arg("x"))

    .def(
        "nextYield",
        &QPot::Static::nextYield,
        "Get the next yield position in either direction.",
        py::arg("offset"))

    .def(
        "currentYieldLeft",
        py::overload_cast<>(&QPot::Static::currentYieldLeft, py::const_),
        "Get the yielding position left")

    .def(
        "currentYieldRight",
        py::overload_cast<>(&QPot::Static::currentYieldRight, py::const_),
        "Get the yielding position right")

    .def(
        "currentYieldLeft",
        py::overload_cast<size_t>(&QPot::Static::currentYieldLeft, py::const_),
        "Get the yielding position left",
        py::arg("offset"))

    .def(
        "currentYieldRight",
        py::overload_cast<size_t>(&QPot::Static::currentYieldRight, py::const_),
        "Get the yielding position right",
        py::arg("offset"))

    .def("checkYieldBoundLeft", &QPot::Static::checkYieldBoundLeft,
        "Check if the particle is 'n' wells from the far-left",
        py::arg("n") = 0)

    .def("checkYieldBoundRight", &QPot::Static::checkYieldBoundRight,
        "Check if the particle is 'n' wells from the far-right",
        py::arg("n") = 0)

    .def(
        "currentYield",
        &QPot::Static::currentYield,
        "Range of yielding positions relative to the current yielding position.",
        py::arg("left"),
        py::arg("right"))

    .def("currentIndex", &QPot::Static::currentIndex,
        "Get the index of the current minimum. Note:"
        "- 'index' : yielding position left"
        "- 'index + 1' : yielding position right")

    .def("__repr__", [](const QPot::Static&) {
        return "<QPot.Static>";
    });

py::class_<QPot::RedrawList>(m, "RedrawList")

    .def(
        py::init<
            xt::xtensor<double,1>,
            std::function<xt::xtensor<double,2>(std::vector<size_t>)>,
            size_t,
            size_t,
            size_t>(),
        "RedrawList",
        py::arg("x"),
        py::arg("redraw_function"),
        py::arg("ntotal") = 1000,
        py::arg("nbuffer") = 300,
        py::arg("noffset") = 20)

    .def(
        "yield",
        py::overload_cast<>(&QPot::RedrawList::yield, py::const_),
        "Yield positions")

    .def(
        "yield",
        py::overload_cast<size_t>(&QPot::RedrawList::yield, py::const_),
        "Specific yield position",
        py::arg("i"))

    .def(
        "yield",
        py::overload_cast<size_t, size_t>(&QPot::RedrawList::yield, py::const_),
        "Range of yield position",
        py::arg("start"),
        py::arg("stop"))

    .def("setProximity", &QPot::RedrawList::setProximity,
        "Customise proximity search",
        py::arg("proximity"))

    .def("setPosition", &QPot::RedrawList::setPosition<xt::xtensor<double, 1>>,
        "Update current position",
        py::arg("x"))

    .def(
        "nextYield",
        &QPot::RedrawList::nextYield,
        "Get the next yield position in either direction.",
        py::arg("offset"))

    .def(
        "currentYieldLeft",
        py::overload_cast<>(&QPot::RedrawList::currentYieldLeft, py::const_),
        "Get the yielding position left")

    .def(
        "currentYieldRight",
        py::overload_cast<>(&QPot::RedrawList::currentYieldRight, py::const_),
        "Get the yielding position right")

    .def(
        "currentYieldLeft",
        py::overload_cast<size_t>(&QPot::RedrawList::currentYieldLeft, py::const_),
        "Get the yielding position left",
        py::arg("offset"))

    .def(
        "currentYieldRight",
        py::overload_cast<size_t>(&QPot::RedrawList::currentYieldRight, py::const_),
        "Get the yielding position right",
        py::arg("offset"))

    .def(
        "currentYield",
        &QPot::RedrawList::currentYield,
        "Range of yielding positions relative to the current yielding position.",
        py::arg("left"),
        py::arg("right"))

    .def("currentIndex", &QPot::RedrawList::currentIndex,
        "Get the index of the current minimum.")

    .def("__repr__", [](const QPot::RedrawList&) {
        return "<QPot.RedrawList>";
    });

}
