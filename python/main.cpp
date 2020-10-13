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

namespace py = pybind11;

PYBIND11_MODULE(QPot, m)
{

m.doc() = "Library to keep track of a sequential potential energy landscape.";

py::class_<QPot::Static>(m, "Static")

    .def(py::init<double, xt::xtensor<double,1>>(),
        "Static yield sequence.", py::arg("x"), py::arg("yield"))

    .def("setProximity", &QPot::Static::setProximity,
        "Customise proximity search",
        py::arg("proximity"))

    .def("setPosition", &QPot::Static::setPosition,
        "Update current position",
        py::arg("x"))

    .def("currentYieldLeft", &QPot::Static::currentYieldLeft,
        "Get the yielding position left")

    .def("currentYieldRight", &QPot::Static::currentYieldRight,
        "Get the yielding position right")

    .def("checkYieldBoundLeft", &QPot::Static::checkYieldBoundLeft,
        "Check if the particle is 'n' wells from the far-left",
        py::arg("n") = 0)

    .def("checkYieldBoundRight", &QPot::Static::checkYieldBoundRight,
        "Check if the particle is 'n' wells from the far-right",
        py::arg("n") = 0)

    .def("currentIndex", &QPot::Static::currentIndex,
        "Get the index of the current minimum. Note:"
        "- 'index' : yielding position left"
        "- 'index + 1' : yielding position right")

    .def("__repr__", [](const QPot::Static&) {
        return "<QPot.Static>";
    });

py::class_<QPot::RedrawList>(m, "RedrawList")

    .def(py::init<xt::xtensor<double,1>,
                  std::function<xt::xtensor<double,2>(std::vector<size_t>)>,
                  size_t,
                  size_t,
                  size_t>(),
        "RedrawList", py::arg("x"), py::arg("redraw_function"), py::arg("ntotal") = 1000, py::arg("nbuffer") = 300, py::arg("noffset") = 20)

    .def("setProximity", &QPot::RedrawList::setProximity,
        "Customise proximity search",
        py::arg("proximity"))

    .def("setPosition", &QPot::RedrawList::setPosition<xt::xtensor<double,1>>,
        "Update current position",
        py::arg("x"))

    .def("currentYieldLeft", &QPot::RedrawList::currentYieldLeft,
        "Get the yielding position left")

    .def("currentYieldRight", &QPot::RedrawList::currentYieldRight,
        "Get the yielding position right")

    .def("currentIndex", &QPot::RedrawList::currentIndex,
        "Get the index of the current minimum.")

    .def("__repr__", [](const QPot::RedrawList&) {
        return "<QPot.RedrawList>";
    });

}
