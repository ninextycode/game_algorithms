#ifndef CFR_H
#define CFR_H

#include <vector>
#include <unordered_map>
#include <GameNode.h>
#include <InfoSet.h>
#include <memory>

using namespace std;


class CFR {
public:
    CFR(shared_ptr<const GameNode> root_node);

    // returns game utility at the root node for player 0 
    // if playing regretsum-based strategy
    double evaluateAndUpdateAll();
    
    InfoSetMap getStrategyInfoSets();

protected:
    virtual shared_ptr<InfoSet> createInfoSet(int n_actions);

private:
    const shared_ptr<const GameNode> root_node;

    // run with no_accumulate to initialise instantaneous regrets only
    double evaluateAndUpdateFromNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0,
        double p_past_actions_p1,
        double p_past_chances,
        bool no_accumulate
    );

    double evaluateAndUpdateFromChanceNode(
        const shared_ptr<const GameNode> node,
        double p_past_actions_p0,
        double p_past_actions_p1,
        double p_past_chances,
        bool no_accumulate
    );

    void initInfoStates();
    void initInfoStatesRecursively(const shared_ptr<const GameNode> node);
    
    InfoSetMap infosets;
};


class CFRPlus : public CFR {
    using CFR::CFR;

protected:
    shared_ptr<InfoSet> createInfoSet(int n_actions) override;
};

#endif  // CFR_H