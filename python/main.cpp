/*

(c - MIT) T.W.J. de Geus (Tom) | www.geus.me | github.com/tdegeus/QPot

*/

#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pyxtensor/pyxtensor.hpp>

#include <QPot/Chunked.hpp>
#include <QPot/Static.hpp>
#include <QPot/Redraw.hpp>
#include <QPot/random.hpp>
#include <QPot/version.hpp>

namespace py = pybind11;

PYBIND11_MODULE(QPot, m)
{

    m.doc() = "Library to keep track of a sequential potential energy landscape.";

    m.def("version", &QPot::version, "version");
    m.def("version_dependencies", &QPot::version_dependencies, "version_dependencies");

    py::class_<QPot::Chunked>(m, "Chunked")

        .def(py::init<>(),
             "Chunked yield sequence."
             "See :cpp:class:`QPot::Chunked`.")

        .def(py::init<double, std::vector<double>, long>(),
             "Chunked yield sequence."
             "See :cpp:class:`QPot::Chunked`.",
             py::arg("x"),
             py::arg("y"),
             py::arg("istart") = 0)

        .def("i", &QPot::Chunked::i,
             "Get the index of the current minimum. Note:"
             "- 'index' : yielding position left"
             "- 'index + 1' : yielding position right."
             "See :cpp:func:`QPot::Chunked::i`.")

        .def("i_chunk", &QPot::Chunked::i_chunk,
             "Get i() relative to the current chunk."
             "See :cpp:func:`QPot::Chunked::i_chunk`.")

        .def("search_proximity", &QPot::Chunked::search_proximity,
             "Customise proximity search."
            "See :cpp:func:`QPot::Chunked::search_proximity`.",
             py::arg("proximity"))

        .def("istart", &QPot::Chunked::istart,
             "Starting index of the chunk."
             "See :cpp:func:`QPot::Chunked::istart`.")

        .def("istop", &QPot::Chunked::istop,
             "Stopping index of the chunk."
             "See :cpp:func:`QPot::Chunked::istop`.")

        .def("ymin", &QPot::Chunked::ymin,
             "First yield position in the chunk."
             "See :cpp:func:`QPot::Chunked::ymin`.")

        .def("ymax", &QPot::Chunked::ymax,
             "Last yield position in the chunk."
             "See :cpp:func:`QPot::Chunked::ymax`.")

        .def("size", &QPot::Chunked::size,
             "Size of the current the chunk."
             "See :cpp:func:`QPot::Chunked::size`.")

        .def("y", &QPot::Chunked::y,
             "Return the chunk."
             "See :cpp:func:`QPot::Chunked::y`.")

        .def("Y", py::overload_cast<long, long>(&QPot::Chunked::Y, py::const_),
             "Return of slice of the chunk."
             "See :cpp:func:`QPot::Chunked::Y(long, long)`.",
             py::arg("start"),
             py::arg("stop"))

        .def("Y", py::overload_cast<long>(&QPot::Chunked::Y, py::const_),
             "Return of slice of the chunk."
             "See :cpp:func:`QPot::Chunked::Y(long)`.",
             py::arg("i"))

        .def("set_y",
             py::overload_cast<long, const std::vector<double>&>(&QPot::Chunked::set_y<std::vector<double>>),
             "Reset the chunk."
             "See :cpp:func:`QPot::Chunked::set_y(long, const T&)`.",
             py::arg("istart"),
             py::arg("y"))

        .def("set_y",
             py::overload_cast<const std::vector<double>&, long>(&QPot::Chunked::set_y<std::vector<double>>),
             "Reset the chunk."
             "See :cpp:func:`QPot::Chunked::set_y(const T&, long)`.",
             py::arg("y"),
             py::arg("istart") = 0)

        .def("rshift_y",
             py::overload_cast<long, const std::vector<double>&, size_t>(&QPot::Chunked::rshift_y<std::vector<double>>),
             "Right-shift chunk."
             "See :cpp:func:`QPot::Chunked::rshift_y(long, const T&, size_t)`.",
             py::arg("istart"),
             py::arg("y"),
             py::arg("nbuffer") = 0)

        .def("rshift_y",
             py::overload_cast<const std::vector<double>&, size_t>(&QPot::Chunked::rshift_y<std::vector<double>>),
             "Right-shift chunk."
             "See :cpp:func:`QPot::Chunked::rshift_y(const T&, size_t)`.",
             py::arg("y"),
             py::arg("nbuffer") = 0)

        .def("rshift_dy",
             py::overload_cast<long, const std::vector<double>&, size_t>(&QPot::Chunked::rshift_dy<std::vector<double>>),
             "Right-shift chunk."
             "See :cpp:func:`QPot::Chunked::rshift_dy(long, const T&, size_t)`.",
             py::arg("istart"),
             py::arg("dy"),
             py::arg("nbuffer") = 0)

        .def("rshift_dy",
             py::overload_cast<const std::vector<double>&, size_t>(&QPot::Chunked::rshift_dy<std::vector<double>>),
             "Right-shift chunk."
             "See :cpp:func:`QPot::Chunked::rshift_dy(const T&, size_t)`.",
             py::arg("dy"),
             py::arg("nbuffer") = 0)

        .def("lshift_y",
             py::overload_cast<long, const std::vector<double>&, size_t>(&QPot::Chunked::lshift_y<std::vector<double>>),
             "Left-shift chunk."
             "See :cpp:func:`QPot::Chunked::lshift_y(long, const T&, size_t)`.",
             py::arg("istart"),
             py::arg("y"),
             py::arg("nbuffer") = 0)

        .def("lshift_y",
             py::overload_cast<const std::vector<double>&, size_t>(&QPot::Chunked::lshift_y<std::vector<double>>),
             "Left-shift chunk."
             "See :cpp:func:`QPot::Chunked::lshift_y(const T&, size_t)`.",
             py::arg("y"),
             py::arg("nbuffer") = 0)

        .def("lshift_dy",
             py::overload_cast<long, const std::vector<double>&, size_t>(&QPot::Chunked::lshift_dy<std::vector<double>>),
             "Left-shift chunk."
             "See :cpp:func:`QPot::Chunked::lshift_dy(long, const T&, size_t)`.",
             py::arg("istart"),
             py::arg("dy"),
             py::arg("nbuffer") = 0)

        .def("lshift_dy",
             py::overload_cast<const std::vector<double>&, size_t>(&QPot::Chunked::lshift_dy<std::vector<double>>),
             "Left-shift chunk."
             "See :cpp:func:`QPot::Chunked::lshift_dy(const T&, size_t)`.",
             py::arg("dy"),
             py::arg("nbuffer") = 0)

        .def("shift_y",
             py::overload_cast<long, const std::vector<double>&, size_t>(&QPot::Chunked::shift_y<std::vector<double>>),
             "Shift chunk."
             "See :cpp:func:`QPot::Chunked::shift_y(long, const T&, size_t)`.",
             py::arg("istart"),
             py::arg("y"),
             py::arg("nbuffer") = 0)

        .def("shift_dy",
             py::overload_cast<long, const std::vector<double>&, size_t>(&QPot::Chunked::shift_dy<std::vector<double>>),
             "Shift chunk."
             "See :cpp:func:`QPot::Chunked::shift_dy(long, const T&, size_t)`.",
             py::arg("istart"),
             py::arg("dy"),
             py::arg("nbuffer") = 0)

        .def("x", &QPot::Chunked::x,
             "Return the position."
             "See :cpp:func:`QPot::Chunked::x`.")

        .def("set_x", &QPot::Chunked::set_x,
             "Set the position."
             "See :cpp:func:`QPot::Chunked::set_x`.",
             py::arg("x"))

        .def("redraw", py::overload_cast<>(&QPot::Chunked::redraw, py::const_),
             "Check for redraw."
             "See :cpp:func:`QPot::Chunked::redraw`.")

        .def("redraw", py::overload_cast<double>(&QPot::Chunked::redraw, py::const_),
             "Check for redraw based on a trial position."
             "See :cpp:func:`QPot::Chunked::redraw`.",
             py::arg("x"))

        .def("yleft", py::overload_cast<>(&QPot::Chunked::yleft, py::const_),
             "Yield position directly left of x()."
             "See :cpp:func:`QPot::Chunked::yleft`.")

        .def("yright", py::overload_cast<>(&QPot::Chunked::yright, py::const_),
             "Yield position directly right of x()."
             "See :cpp:func:`QPot::Chunked::yright`.")

        .def("yleft", py::overload_cast<size_t>(&QPot::Chunked::yleft, py::const_),
             "Yield position at an offset left of x()."
             "See :cpp:func:`QPot::Chunked::yleft`.",
             py::arg("offset"))

        .def("yright", py::overload_cast<size_t>(&QPot::Chunked::yright, py::const_),
             "Yield position at an offset right of x()."
             "See :cpp:func:`QPot::Chunked::yright`.",
             py::arg("offset"))

        .def("boundcheck_left", &QPot::Chunked::boundcheck_left,
             "Check that x() is at least `n` yield positions from the far-left."
             "See :cpp:func:`QPot::Chunked::boundcheck_left`.",
             py::arg("n") = 0)

        .def("boundcheck_right", &QPot::Chunked::boundcheck_right,
             "Check that x() is at least `n` yield positions from the far-right."
             "See :cpp:func:`QPot::Chunked::boundcheck_right`.",
             py::arg("n") = 0)

        .def("__repr__", [](const QPot::Chunked&) { return "<QPot.Chunked>"; });

    py::class_<QPot::Static>(m, "Static")

        .def(py::init<double, xt::xtensor<double, 1>>(),
             "Static yield sequence.",
             py::arg("x"),
             py::arg("yield"))

        .def("yieldPosition",
             py::overload_cast<>(&QPot::Static::yieldPosition, py::const_),
             "Yield positions")

        .def("yieldPosition",
             py::overload_cast<size_t>(&QPot::Static::yieldPosition, py::const_),
             "Specific yield position",
             py::arg("i"))

        .def("yieldPosition",
             py::overload_cast<size_t, size_t>(&QPot::Static::yieldPosition, py::const_),
             "Range of yield positions",
             py::arg("start"),
             py::arg("stop"))

        .def("setProximity", &QPot::Static::setProximity,
             "Customise proximity search",
             py::arg("proximity"))

        .def("setPosition", &QPot::Static::setPosition,
             "Update current position",
             py::arg("x"))

        .def("nextYield",
             &QPot::Static::nextYield,
             "Get the next yield position in either direction.",
             py::arg("offset"))

        .def("currentYieldLeft",
             py::overload_cast<>(&QPot::Static::currentYieldLeft, py::const_),
             "Get the yielding position left")

        .def("currentYieldRight",
             py::overload_cast<>(&QPot::Static::currentYieldRight, py::const_),
             "Get the yielding position right")

        .def("currentYieldLeft",
             py::overload_cast<size_t>(&QPot::Static::currentYieldLeft, py::const_),
             "Get the yielding position left",
             py::arg("offset"))

        .def("currentYieldRight",
             py::overload_cast<size_t>(&QPot::Static::currentYieldRight, py::const_),
             "Get the yielding position right",
             py::arg("offset"))

        .def("checkYieldBoundLeft", &QPot::Static::checkYieldBoundLeft,
             "Check if the particle is 'n' wells from the far-left",
             py::arg("n") = 0)

        .def("checkYieldBoundRight", &QPot::Static::checkYieldBoundRight,
             "Check if the particle is 'n' wells from the far-right",
             py::arg("n") = 0)

        .def("currentYield",
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

        .def(py::init<xt::xtensor<double, 1>,
                     std::function<xt::xtensor<double, 2>(std::array<size_t, 2>)>,
                     size_t,
                     size_t,
                     size_t>(),
             "RedrawList",
             py::arg("x"),
             py::arg("redraw_function"),
             py::arg("ntotal") = 1000,
             py::arg("nbuffer") = 300,
             py::arg("noffset") = 20)

        .def("yieldPosition",
             py::overload_cast<>(&QPot::RedrawList::yieldPosition, py::const_),
             "Yield positions")

        .def("yieldPosition",
             py::overload_cast<size_t>(&QPot::RedrawList::yieldPosition, py::const_),
             "Specific yield position",
             py::arg("i"))

        .def("yieldPosition",
             py::overload_cast<size_t, size_t>(&QPot::RedrawList::yieldPosition, py::const_),
             "Range of yield position",
             py::arg("start"),
             py::arg("stop"))

        .def("setProximity", &QPot::RedrawList::setProximity,
             "Customise proximity search",
             py::arg("proximity"))

        .def("setPosition", &QPot::RedrawList::setPosition<xt::xtensor<double, 1>>,
             "Update current position",
             py::arg("x"))

        .def("nextYield",
             &QPot::RedrawList::nextYield,
             "Get the next yield position in either direction.",
             py::arg("offset"))

        .def("currentYieldLeft",
             py::overload_cast<>(&QPot::RedrawList::currentYieldLeft, py::const_),
             "Get the yielding position left")

        .def("currentYieldRight",
             py::overload_cast<>(&QPot::RedrawList::currentYieldRight, py::const_),
             "Get the yielding position right")

        .def("currentYieldLeft",
             py::overload_cast<size_t>(&QPot::RedrawList::currentYieldLeft, py::const_),
             "Get the yielding position left",
             py::arg("offset"))

        .def("currentYieldRight",
             py::overload_cast<size_t>(&QPot::RedrawList::currentYieldRight, py::const_),
             "Get the yielding position right",
             py::arg("offset"))

        .def("currentYield",
             &QPot::RedrawList::currentYield,
             "Range of yielding positions relative to the current yielding position.",
             py::arg("left"),
             py::arg("right"))

        .def("currentIndex", &QPot::RedrawList::currentIndex,
             "Get the index of the current minimum.")

        .def("currentAnyRedraw", &QPot::RedrawList::currentAnyRedraw,
             "Check if there was a redraw the last time setPosition() was called.")

        .def("currentRedraw", &QPot::RedrawList::currentRedraw,
             "Get the direction of redrawing the last time setPosition() was called.")

        .def("currentRedrawRight", &QPot::RedrawList::currentRedrawRight,
             "List of particles that were redrawn to right, the last time setPosition() was called.")

        .def("currentRedrawLeft", &QPot::RedrawList::currentRedrawLeft,
             "List of particles that were redrawn to left, the last time setPosition() was called.")

        .def("redraw", &QPot::RedrawList::redraw<xt::xtensor<int, 1>>,
             "Force redraw (can be used to restore a sequence).",
             py::arg("iredraw"))

        .def("redrawRight", &QPot::RedrawList::redrawRight<xt::xtensor<size_t, 1>>,
             "Force redraw (can be used to restore a sequence).",
             py::arg("index"))

        .def("redrawLeft", &QPot::RedrawList::redrawLeft<xt::xtensor<size_t, 1>>,
             "Force redraw (can be used to restore a sequence).",
             py::arg("index"))

        .def("raw_val", &QPot::RedrawList::raw_val,
             "Current matrix of random values.")

        .def("raw_pos", &QPot::RedrawList::raw_pos,
             "Current matrix of yielding positions.")

        .def("raw_idx", &QPot::RedrawList::raw_idx,
             "Current matrix of indices.")

        .def("raw_idx_t", &QPot::RedrawList::raw_idx_t,
             "Current matrix of historic indices.")

        .def("__repr__", [](const QPot::RedrawList&) {
            return "<QPot.RedrawList>";
        });

    // -----------
    // QPot.random
    // -----------

    {
        py::module sm = m.def_submodule("random", "random");

        namespace SM = QPot::random;

        sm.def("seed",
               &SM::seed<size_t>,
               "Set seed.",
               py::arg("seed"));

        sm.def("UniformList",
               &SM::UniformList,
               "Function to a matrix (rank 2 array) of ones.",
               py::arg("scale") = 1.0);

        sm.def("RandList",
               &SM::RandList,
               "Function to a matrix (rank 2 array) from a uniform distribution.",
               py::arg("scale") = 1.0);

        sm.def("WeibullList",
               &SM::WeibullList,
               "Function to a matrix (rank 2 array) from a Weibull distribution.",
               py::arg("a") = 1.0,
               py::arg("b") = 1.0);

        sm.def("GammaList",
               &SM::GammaList,
               "Function to a matrix (rank 2 array) from a Gamma distribution.",
               py::arg("alpha") = 1.0,
               py::arg("beta") = 1.0);
    }

}
