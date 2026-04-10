#include "abstract/regret_data/regret_data.h"
#include "abstract/regret_data/regret_data_map.h"
#include "abstract/nodes/game_node.h"


namespace strategy_utils {
    // returns node value for player 0 under self-play with infoset cumulative strategy
    template <RegretDataKey RKey>
    double evaluateNode(
        shared_ptr<const GameNode> node,
        const RegretDataMap<RKey>& regret_data_map
    );
}

#include "abstract/strategy/utils.hpp"