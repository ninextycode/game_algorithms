#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/operators.h>
#include <memory>

#include "cfr/CFRPlus.h"
#include "abstract/infoset/InfoSet.h"
#include "abstract/infoset/InfoSetMap.h"
#include "abstract/infoset/InfoSetUtils.h"
#include "abstract/nodes/GameNode.h"
#include "abstract/nodes/Randomizer.h"
#include "tictactoe/TicTacToeBoard.h"
#include "tictactoe/TicTacToeNode.h"
#include "tictactoe/TTTInvariant.h"
#include "pybind/PyGameNode.h"
#include "Utils.h"

namespace py = pybind11;
using namespace std;

PYBIND11_MODULE(game_algorithms_py, m) {
    m.doc() = "Game algorithms library";

    // GameNode enum and base class
    py::enum_<GameNode::Type>(m, "GameNodeType")
        .value("Terminal", GameNode::Type::Terminal)
        .value("Chance", GameNode::Type::Chance)
        .value("Decision", GameNode::Type::Decision);

    py::class_<GameNode, PyGameNode, shared_ptr<GameNode>>(m, "GameNode")
        .def(py::init<>())
        .def("getType", &GameNode::getType)
        .def("getTypeString", &GameNode::getTypeString)
        .def("getTerminalUtilities", &GameNode::getTerminalUtilities, 
             py::return_value_policy::reference_internal)
        .def("getChanceProbabilities", &GameNode::getChanceProbabilities,
             py::return_value_policy::reference_internal)
        .def("getLegalActions", &GameNode::getLegalActions,
             py::return_value_policy::reference_internal)
        .def("applyAction", &GameNode::applyAction)
        .def("getCurrentPlayer", &GameNode::getCurrentPlayer)
        .def("getInfoSetKeyString", &GameNode::getInfoSetKeyString)
        .def("getInfoSetKeyInt", &GameNode::getInfoSetKeyInt);

    // TicTacToeBoard
    py::class_<TicTacToeBoard>(m, "TicTacToeBoard")
        .def(py::init<>())
        .def(py::init<const array<array<int, 3>, 3>&>())
        .def(py::init<const array<int, 9>&>())
        .def("copy", &TicTacToeBoard::copy)
        .def("verticalFlip", &TicTacToeBoard::verticalFlip)
        .def("horizontalFlip", &TicTacToeBoard::horizontalFlip)
        .def("rotateClockwise", &TicTacToeBoard::rotateClockwise)
        .def("rotateCounterclockwise", &TicTacToeBoard::rotateCounterclockwise)
        .def("get", py::overload_cast<int, int>(&TicTacToeBoard::get, py::const_))
        .def("get", py::overload_cast<int>(&TicTacToeBoard::get, py::const_))
        .def("getBoard", &TicTacToeBoard::getBoard)
        .def("getFlatBoard", &TicTacToeBoard::getFlatBoard)
        .def("getCurrentPlayer", &TicTacToeBoard::getCurrentPlayer)
        .def("toString", &TicTacToeBoard::toString)
        .def("isEmpty", py::overload_cast<int, int>(&TicTacToeBoard::isEmpty, py::const_))
        .def("isEmpty", py::overload_cast<int>(&TicTacToeBoard::isEmpty, py::const_))
        .def("makeMove", py::overload_cast<int, int>(&TicTacToeBoard::makeMove))
        .def("makeMove", py::overload_cast<int>(&TicTacToeBoard::makeMove))
        .def("getNormalForm", &TicTacToeBoard::getNormalForm)
        .def("isTerminal", &TicTacToeBoard::isTerminal)
        .def("getWinner", &TicTacToeBoard::getWinner)
        .def("isFull", &TicTacToeBoard::isFull)
        .def("getPossibleActions", &TicTacToeBoard::getPossibleActions)
        .def("getUniqueInvariantActions", &TicTacToeBoard::getUniqueInvariantActions)
        .def_static("fromString", &TicTacToeBoard::fromString)
        .def("__lt__", &TicTacToeBoard::operator<)
        .def("__eq__", &TicTacToeBoard::operator==)
        .def("__str__", &TicTacToeBoard::toString)
        .def("__repr__", [](const TicTacToeBoard& board) {
            return "TicTacToeBoard:\n" + board.toString();
        });

    // TicTacToeNode
    py::class_<TicTacToeNode, GameNode, shared_ptr<TicTacToeNode>>(m, "TicTacToeNode")
        .def(py::init<>())
        .def(py::init<const TicTacToeBoard&>())
        .def("getBoardString", &TicTacToeNode::getBoardString)
        .def("getBoard", &TicTacToeNode::getBoard, py::return_value_policy::reference_internal);

    // TTTInvariant
    py::class_<TTTInvariant, GameNode, shared_ptr<TTTInvariant>>(m, "TTTInvariant")
        .def(py::init<>())
        .def(py::init<const TicTacToeNode&>());

    // Randomizer wrapper classes
    py::class_<RandomizerWrapNode, GameNode, shared_ptr<RandomizerWrapNode>>(m, "RandomizerWrapNode")
        .def(py::init<shared_ptr<const GameNode>, double, int>(),
             py::arg("wrapped_node"), py::arg("p_random_choice"), py::arg("max_random_actions") = -1)
        .def_readonly_static("NONRANDOM_CHOICE", &RandomizerWrapNode::NONRANDOM_CHOICE)
        .def_readonly_static("RANDOM_CHOICE", &RandomizerWrapNode::RANDOM_CHOICE);

    py::class_<NonRandomDecisionWrapNode, GameNode, shared_ptr<NonRandomDecisionWrapNode>>(m, "NonRandomDecisionWrapNode")
        .def(py::init<shared_ptr<const GameNode>, double, int>(),
             py::arg("wrapped_node"), py::arg("p_random_choice"), py::arg("max_random_actions") = -1);

    py::class_<RandomDecisionWrapNode, GameNode, shared_ptr<RandomDecisionWrapNode>>(m, "RandomDecisionWrapNode")
        .def(py::init<shared_ptr<const GameNode>, double, size_t>(),
             py::arg("wrapped_node"), py::arg("p_random_choice"), py::arg("max_random_actions") = -1);

    py::class_<ChanceWrapNode, GameNode, shared_ptr<ChanceWrapNode>>(m, "ChanceWrapNode")
        .def(py::init<shared_ptr<const GameNode>, double, size_t>(),
             py::arg("wrapped_node"), py::arg("p_random_choice"), py::arg("max_random_actions") = -1);

    // InfoSet
    py::class_<InfoSet>(m, "InfoSet")
        .def(py::init<int>())
        .def(py::init<const InfoSet&>())
        .def("getInstantRegret", &InfoSet::getInstantRegret,
             py::return_value_policy::reference_internal)
        .def("getRegretSum", &InfoSet::getRegretSum,
             py::return_value_policy::reference_internal)
        .def("getRegretSumStrategy", &InfoSet::getRegretSumStrategy,
             py::return_value_policy::reference_internal)
        .def("getCumulativeStrategy", &InfoSet::getCumulativeStrategy,
             py::return_value_policy::reference_internal)
        .def("setInstantRegret", &InfoSet::setInstantRegret)
        .def("accumulateRegret", &InfoSet::accumulateRegret)
        .def("accumulateStrategy", &InfoSet::accumulateStrategy);

    // InfoSetMap types - factory functions that return new dict instances
    m.def("InfoSetMapStr", []() { return py::dict(); }, "Create a new string-keyed InfoSet dictionary");
    m.def("InfoSetMapInt", []() { return py::dict(); }, "Create a new int-keyed InfoSet dictionary");

    // InfoSet utilities
    py::class_<infoset_utils::RegretMetric>(m, "RegretMetric")
        .def_readwrite("n_regrets", &infoset_utils::RegretMetric::n_regrets)
        .def_readwrite("max_instant_regret", &infoset_utils::RegretMetric::max_instant_regret)
        .def_readwrite("sum_positive_instant_regrets", &infoset_utils::RegretMetric::sum_positive_instant_regrets);

    // InfoSet SaveLoader utilities
    py::class_<infoset_utils::SaveLoader>(m, "InfoSetSaveLoader")
        .def_static("saveInfoSetMapStrategy", 
                    py::overload_cast<const InfoSetMap<string>&, const string&>(
                        &infoset_utils::SaveLoader::saveInfoSetMapStrategy<string>
                    ))
        .def_static("saveInfoSetMapStrategy", 
                    py::overload_cast<const InfoSetMap<size_t>&, const string&>(
                        &infoset_utils::SaveLoader::saveInfoSetMapStrategy<size_t>
                    ))
        .def_static("saveInfoSetMapRegretSum", 
                    py::overload_cast<const InfoSetMap<string>&, const string&>(
                        &infoset_utils::SaveLoader::saveInfoSetMapRegretSum<string>
                    ))
        .def_static("saveInfoSetMapRegretSum", 
                    py::overload_cast<const InfoSetMap<size_t>&, const string&>(
                        &infoset_utils::SaveLoader::saveInfoSetMapRegretSum<size_t>
                    ))
        .def_static("saveInfoSetMapInstantRegret", 
                    py::overload_cast<const InfoSetMap<string>&, const string&>(
                        &infoset_utils::SaveLoader::saveInfoSetMapInstantRegret<string>
                    ))
        .def_static("saveInfoSetMapInstantRegret", 
                    py::overload_cast<const InfoSetMap<size_t>&, const string&>(
                        &infoset_utils::SaveLoader::saveInfoSetMapInstantRegret<size_t>
                    ))
        .def_static("loadInfoSetMap", 
                    py::overload_cast<const string&, const string&>(
                        &infoset_utils::SaveLoader::loadInfoSetMap<string>
                    ))
        .def_static("loadInfoSetMapInt", 
                    py::overload_cast<const string&, const string&>(
                        &infoset_utils::SaveLoader::loadInfoSetMap<size_t>
                    ));
    
    // InfoSet Converter utilities
    py::class_<infoset_utils::Converter>(m, "InfoSetConverter")
        .def_static("convertToString", &infoset_utils::Converter::convertTo<string, size_t>)
        .def_static("convertToInt", &infoset_utils::Converter::convertTo<size_t, string>);

    // Metric calculation utilities
    m.def("calculateMetricStr", &infoset_utils::calculateMetric<string>);
    m.def("calculateMetricInt", &infoset_utils::calculateMetric<size_t>);


    // CFRPlus classes with nested Builders
    auto cfrplus_str = py::class_<CFRPlus<string>>(m, "CFRPlusStr")
        .def(py::init<shared_ptr<const GameNode>, bool, double, const InfoSetMap<string>&>(),
             py::arg("root_node"), py::arg("initial_evaluation_run") = true,
             py::arg("e_soft_regsum_strategies") = 0.0,
             py::arg("initial_state") = InfoSetMap<string>())
        .def("evaluateAndUpdate", &CFRPlus<string>::evaluateAndUpdate,
             py::arg("accumulate_regsum") = true, py::arg("accumulate_strategy") = true)
        .def("evaluate", &CFRPlus<string>::evaluate)
        .def("getStrategyInfoSets", &CFRPlus<string>::getStrategyInfoSets,
             py::return_value_policy::reference_internal)
        .def("processNode", &CFRPlus<string>::processNode,
             py::arg("node"), py::arg("p_past_actions_p0") = 1.0,
             py::arg("p_past_actions_p1") = 1.0, py::arg("p_past_chances") = 1.0,
             py::arg("accumulate_regsum") = false, py::arg("accumulate_strategy") = false);

    py::class_<CFRPlus<string>::Builder>(cfrplus_str, "Builder")
        .def(py::init<>())
        .def("setRootNode", &CFRPlus<string>::Builder::setRootNode,
             py::return_value_policy::reference_internal)
        .def("setInitialEvaluationRun", &CFRPlus<string>::Builder::setInitialEvaluationRun,
             py::return_value_policy::reference_internal)
        .def("setESoftRegsumStrategies", &CFRPlus<string>::Builder::setESoftRegsumStrategies,
             py::return_value_policy::reference_internal)
        .def("setInitialState", &CFRPlus<string>::Builder::setInitialState,
             py::return_value_policy::reference_internal)
        .def("buildCfr", &CFRPlus<string>::Builder::buildCfr);

    auto cfrplus_int = py::class_<CFRPlus<size_t>>(m, "CFRPlusInt")
        .def(py::init<shared_ptr<const GameNode>, bool, double, const InfoSetMap<size_t>&>(),
             py::arg("root_node"), py::arg("initial_evaluation_run") = true,
             py::arg("e_soft_regsum_strategies") = 0.0,
             py::arg("initial_state") = InfoSetMap<size_t>())
        .def("evaluateAndUpdate", &CFRPlus<size_t>::evaluateAndUpdate,
             py::arg("accumulate_regsum") = true, py::arg("accumulate_strategy") = true)
        .def("evaluate", &CFRPlus<size_t>::evaluate)
        .def("getStrategyInfoSets", &CFRPlus<size_t>::getStrategyInfoSets,
             py::return_value_policy::reference_internal)
        .def("processNode", &CFRPlus<size_t>::processNode,
             py::arg("node"), py::arg("p_past_actions_p0") = 1.0,
             py::arg("p_past_actions_p1") = 1.0, py::arg("p_past_chances") = 1.0,
             py::arg("accumulate_regsum") = false, py::arg("accumulate_strategy") = false);

    py::class_<CFRPlus<size_t>::Builder>(cfrplus_int, "Builder")
        .def(py::init<>())
        .def("setRootNode", &CFRPlus<size_t>::Builder::setRootNode,
             py::return_value_policy::reference_internal)
        .def("setInitialEvaluationRun", &CFRPlus<size_t>::Builder::setInitialEvaluationRun,
             py::return_value_policy::reference_internal)
        .def("setESoftRegsumStrategies", &CFRPlus<size_t>::Builder::setESoftRegsumStrategies,
             py::return_value_policy::reference_internal)
        .def("setInitialState", &CFRPlus<size_t>::Builder::setInitialState,
             py::return_value_policy::reference_internal)
        .def("buildCfr", &CFRPlus<size_t>::Builder::buildCfr);

    // Strategy utilities
    m.def("normalizeStrategy", &strategy_utils::normalizeStrategy,
          "Normalize a strategy vector to sum to 1.0");
    m.def("epsilonSoftStrategy", &strategy_utils::epsilonSoftStrategy,
          py::arg("epsilon"), py::arg("strategy"),
          "Create epsilon-soft strategy with minimum probability epsilon/n for each action");
}