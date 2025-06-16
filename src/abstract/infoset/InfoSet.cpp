#include "abstract/infoset/InfoSet.h"
#include <functional>
#include "Utils.h"


InfoSet::InfoSet(int n_actions) 
    : instant_regret_(n_actions, 0.0),
    regret_sum_(n_actions, 20.0), // TODO return to 0
    regret_sum_strategy_uptodate_(false),
    regret_sum_strategy_(n_actions, 0.0),
    cumulative_strategy_not_norm_(n_actions, 0.0),
    cumulative_strategy_uptodate_(false),
    cumulative_strategy_normalized_(n_actions, 0.0)
{
}

InfoSet::InfoSet(const InfoSet& other)
    : instant_regret_(other.instant_regret_),
    regret_sum_(other.regret_sum_),
    regret_sum_strategy_uptodate_(other.regret_sum_strategy_uptodate_),
    regret_sum_strategy_(other.regret_sum_strategy_),
    cumulative_strategy_not_norm_(other.cumulative_strategy_not_norm_),
    cumulative_strategy_uptodate_(other.cumulative_strategy_uptodate_),
    cumulative_strategy_normalized_(other.cumulative_strategy_normalized_)
{
}

void InfoSet::setInstantRegret(int action_idx, double regret) {
    instant_regret_[action_idx] = regret;
}

void InfoSet::accumulateRegret(double weight) {
    for (size_t action_idx = 0; action_idx < instant_regret_.size(); action_idx++) {
        this->regret_sum_[action_idx] += weight * instant_regret_[action_idx];
    }

    // Ensure all regret sums are non-negative
    for (size_t i = 0; i < regret_sum_.size(); i++) {
        regret_sum_[i] = max(0.0, regret_sum_[i]);
    }

    // only when instant regrets are added to cumulative regrets
    // the old regretsum strategy becomes outdated
    regret_sum_strategy_uptodate_ = false;
}

void InfoSet::accumulateStrategy(double weight) {
    const vector<double>& strategy = getRegretSumStrategy();
    for (size_t i = 0; i < strategy.size(); i++) {
        cumulative_strategy_not_norm_[i] += weight * strategy[i];
    }
    cumulative_strategy_uptodate_ = false;
}

const vector<double>& InfoSet::getInstantRegret() const {
    return instant_regret_;
}

const vector<double>& InfoSet::getRegretSum() const {
    return regret_sum_;
}

const vector<double>& InfoSet::getRegretSumStrategy() {
    if (!regret_sum_strategy_uptodate_) {
        // Normalize the strategy - this step can be precomputed
        regret_sum_strategy_ = strategy_utils::normalizeStrategy(regret_sum_);
        regret_sum_strategy_uptodate_ = true;
    }

    return regret_sum_strategy_;
}

#include <iostream>
using namespace std;

const vector<double>& InfoSet::getCumulativeStrategy() {
    if (!cumulative_strategy_uptodate_) {
        cumulative_strategy_normalized_ =\
            strategy_utils::normalizeStrategy(cumulative_strategy_not_norm_);
        cumulative_strategy_uptodate_ = true;
    }
    return cumulative_strategy_normalized_;
}
