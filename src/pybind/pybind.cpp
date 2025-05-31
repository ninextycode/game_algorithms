#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <memory>
#include "CFRPlus.h"
#include "InfoSet.h"
#include "GameNode.h"
#include "tictactoe/TicTacToeBoard.h"
#include "pybind/PyGameNode.h"

namespace py = pybind11;
using namespace std;


PYBIND11_MODULE(game_algorithms_py, m) {
    m.doc() = "Game algorithms library";

    // TicTacToeBoard class
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

}