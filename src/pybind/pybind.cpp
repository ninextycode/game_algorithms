#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include "CFR.h"
#include "InfoSet.h"
#include "GameNode.h"
#include "pybind/PyGameNode.h"

namespace py = pybind11;
using namespace std;


PYBIND11_MODULE(game_algorithms_py, m) {
    m.doc() = "Game algorithms library";

    // PyGameNode trampoline class
    py::class_<GameNode, PyGameNode, shared_ptr<GameNode>>(m, "GameNode")
        .def(py::init<>())
        .def("isTerminal", &GameNode::isTerminal)
        .def("getUtilitiesForTerminal", &GameNode::getUtilitiesForTerminal)
        .def("isChance", &GameNode::isChance)
        .def("getChanceProbabilities", &GameNode::getChanceProbabilities)
        .def("getCurrentPlayer", &GameNode::getCurrentPlayer)
        .def("getAvailableActions", &GameNode::getAvailableActions)
        .def("nextGameNode", &GameNode::nextGameNode)
        .def("getInfoSetKey", &GameNode::getInfoSetKey);

    // InfoSet class - I havent exposed "setter" methods
    py::class_<InfoSet, shared_ptr<InfoSet>>(m, "InfoSet")
        .def(py::init<int>())
        .def(py::init<const InfoSet&>())
        .def("getInstantRegrets", &InfoSet::getInstantRegrets)
        .def("getRegretSum", &InfoSet::getRegretSum)
        .def("getRegretSumStrategy", &InfoSet::getRegretSumStrategy);

    // CFR class
    py::class_<CFR>(m, "CFR")
        .def(py::init<shared_ptr<const GameNode>>())
        .def("evaluateAndUpdateAll", &CFR::evaluateAndUpdateAll)
        .def("getStrategyInfoSets", &CFR::getStrategyInfoSets);
        
    // CFRPlus class
    py::class_<CFRPlus, CFR>(m, "CFRPlus")
        .def(py::init<shared_ptr<const GameNode>>());
}