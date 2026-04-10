#pragma once

#include "abstract/strategy/utils.h"
#include <stdexcept>

template <RegretDataKey RKey>
double strategy_utils::evaluateNode(
    shared_ptr<const GameNode> node,
    const RegretDataMap<RKey>& regret_data_map
) {
    switch (node->getType()) {
        case GameNode::Type::Terminal: {
            const auto& utilities = node->getTerminalUtilities();
            return utilities[0]; // Return utility for player 0
        }
        
        case GameNode::Type::Chance: {
            const auto& probabilities = node->getChanceProbabilities();
            const auto& legal_actions = node->getLegalActions();
            
            double expected_value = 0.0;
            for (size_t i = 0; i < legal_actions.size(); ++i) {
                int action = legal_actions[i];
                auto child_node = node->applyAction(action);
                double child_value = evaluateNode(child_node, regret_data_map);
                expected_value += probabilities[i] * child_value;
            }
            return expected_value;
        }
        
        case GameNode::Type::Decision: {
            const auto& legal_actions = node->getLegalActions();
            RKey infoset_key = node->getInfoSetKey<RKey>();
            
            auto it = regret_data_map.find(infoset_key);
            if (it == regret_data_map.end()) {
                throw runtime_error("InfoSet not found for key in strategy map");
            }
            
            const auto& strategy = it->second.getCumulativeStrategy();
            
            double expected_value = 0.0;
            for (size_t i = 0; i < legal_actions.size(); ++i) {
                int action = legal_actions[i];
                auto child_node = node->applyAction(action);
                double child_value = evaluateNode(child_node, regret_data_map);
                expected_value += strategy[i] * child_value;
            }
            return expected_value;
        }
        
        default:
            throw logic_error("Unknown node type in evaluateNode");
    }
}
