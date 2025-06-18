#include "abstract/infoset/InfoSet.h"
#include "abstract/infoset/InfoSetMap.h"
#include "abstract/nodes/GameNode.h"


namespace strategy_utils {
    // returns node value for player 0 under self-play with infoset cumulative strategy
    template <InfoSetKey ISKey>
    double evaluateNode(
        shared_ptr<const GameNode> node,
        const InfoSetMap<ISKey>& infoset_strategy
    );
}

#include "abstract/strategy/Utils.hpp"