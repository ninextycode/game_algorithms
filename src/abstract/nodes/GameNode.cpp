#include "abstract/nodes/GameNode.h"
#include "Utils.h"
#include <stdexcept>
#include <string>
#include <vector>
#include <memory>
#include <typeinfo>


string GameNode::getTypeString() const {
    Type type = this->getType();
    switch (type) {
        case Type::Terminal: return "Terminal";
        case Type::Chance: return "Chance";
        case Type::Decision: return "Decision";
        default: throw logic_error(
            "Unknown GameNode type "
            "with id = " + to_string(static_cast<int>(type))
        );
    }
}

const vector<double>& GameNode::getTerminalUtilities() const {
    if (getType() == Type::Terminal) {
        throwMissingFnException("getTerminalUtilities");
    } else {
        throwWrongFnException("getTerminalUtilities");
    }
}

const vector<double>& GameNode::getChanceProbabilities() const {
    if (getType() == Type::Chance) {
        throwMissingFnException("getChanceProbabilities");
    } else {
        throwWrongFnException("getChanceProbabilities");
    }
}

const vector<int>& GameNode::getLegalActions() const {
    if (getType() == Type::Chance || getType() == Type::Decision) {
        throwMissingFnException("getLegalActions");
    } else {
        throwWrongFnException("getLegalActions");
    }
}

shared_ptr<const GameNode> GameNode::applyAction(int action) const {
    if (getType() == Type::Chance || getType() == Type::Decision) {
        throwMissingFnException("applyAction");
    } else {
        throwWrongFnException("applyAction");
    }
}

int GameNode::getCurrentPlayer() const {
    if (getType() == Type::Decision) {
        throwMissingFnException("getCurrentPlayer");
    } else {
        throwWrongFnException("getCurrentPlayer");
    }
}

string GameNode::getInfoSetKeyString() const {
    if (getType() == Type::Decision) {
        throwMissingFnException("getInfoSetKeyString");
    } else {
        throwWrongFnException("getInfoSetKeyString");
    }
}

size_t GameNode::getInfoSetKeyInt() const {
    return hash<string>{}(getInfoSetKeyString());
}

void GameNode::throwWrongFnException(const string& funcName) const {
    throw logic_error(
        string("Node of class ") + cpp_utils::demangle(typeid(*this).name()) + ", " +
        "of type " + this->getTypeString() + " does not implement " + funcName + "()"
    );
}

void GameNode::throwMissingFnException(const string& funcName) const {
    throw logic_error(
        string("Node of class ") + cpp_utils::demangle(typeid(*this).name()) + ", " +
        "of type " + this->getTypeString() + " is missing an implementation for " + funcName + "() function"
    );
}
