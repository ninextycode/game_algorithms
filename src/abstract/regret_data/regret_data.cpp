#include "abstract/regret_data/regret_data.h"
#include "utils.h"
#include <stdexcept>


RegretData::RegretData(int n_actions) 
    : instant_regret_(n_actions, 0.0),
    regret_sum_(n_actions, 0.0),
    regret_sum_strategy_uptodate_(false),
    regret_sum_strategy_(n_actions, 0.0),
    cumulative_strategy_not_norm_(n_actions, 0.0),
    cumulative_strategy_uptodate_(false),
    cumulative_strategy_normalized_(n_actions, 0.0)
{
}

RegretData::RegretData(const RegretData& other)
    : instant_regret_(other.instant_regret_),
    regret_sum_(other.regret_sum_),
    regret_sum_strategy_uptodate_(other.regret_sum_strategy_uptodate_),
    regret_sum_strategy_(other.regret_sum_strategy_),
    cumulative_strategy_not_norm_(other.cumulative_strategy_not_norm_),
    cumulative_strategy_uptodate_(other.cumulative_strategy_uptodate_),
    cumulative_strategy_normalized_(other.cumulative_strategy_normalized_)
{
}

RegretData::RegretData(
    const vector<double>& regret_sum,
    const vector<double>& cumulative_strategy_not_norm
)
    : instant_regret_(regret_sum.size(), 0.0),
    regret_sum_(regret_sum),
    regret_sum_strategy_uptodate_(false),
    regret_sum_strategy_(),
    cumulative_strategy_not_norm_(cumulative_strategy_not_norm),
    cumulative_strategy_uptodate_(false),
    cumulative_strategy_normalized_()
{
    if (regret_sum.size() != cumulative_strategy_not_norm.size()) {
        throw invalid_argument("regret_sum and cumulative_strategy_not_norm must have the same size");
    }
}

void RegretData::setInstantRegret(int action_idx, double regret) {
    instant_regret_[action_idx] = regret;
}

void RegretData::accumulateRegret(double weight) {
    for (size_t action_idx = 0; action_idx < instant_regret_.size(); action_idx++) {
        this->regret_sum_[action_idx] += weight * instant_regret_[action_idx];
        instant_regret_[action_idx] = 0.0;
    }

    // Ensure all regret sums are non-negative
    for (size_t i = 0; i < regret_sum_.size(); i++) {
        regret_sum_[i] = max(0.0, regret_sum_[i]);
    }

    // only when instant regrets are added to cumulative regrets
    // the old regretsum strategy becomes outdated
    regret_sum_strategy_uptodate_ = false;
}

void RegretData::accumulateStrategy(double weight) {
    const vector<double>& strategy = getRegretSumStrategy();
    for (size_t i = 0; i < strategy.size(); i++) {
        cumulative_strategy_not_norm_[i] += weight * strategy[i];
    }
    cumulative_strategy_uptodate_ = false;
}

const vector<double>& RegretData::getCumulativeStrategyNotNorm() const {
    return cumulative_strategy_not_norm_;
}

const vector<double>& RegretData::getInstantRegret() const {
    return instant_regret_;
}

const vector<double>& RegretData::getRegretSum() const {
    return regret_sum_;
}

const vector<double>& RegretData::getRegretSumStrategy() {
    if (!regret_sum_strategy_uptodate_) {
        // Normalize the strategy - this step can be precomputed
        regret_sum_strategy_ = strategy_utils::normalizeStrategy(regret_sum_);
        regret_sum_strategy_uptodate_ = true;
    }

    return regret_sum_strategy_;
}

const vector<double>& RegretData::getCumulativeStrategy() {
    if (!cumulative_strategy_uptodate_) {
        cumulative_strategy_normalized_ =\
            strategy_utils::normalizeStrategy(cumulative_strategy_not_norm_);
        cumulative_strategy_uptodate_ = true;
    }
    return cumulative_strategy_normalized_;
}



vector<double> RegretData::getRegretSumStrategy() const {
    return strategy_utils::normalizeStrategy(regret_sum_);
}

vector<double> RegretData::getCumulativeStrategy() const {
    return strategy_utils::normalizeStrategy(cumulative_strategy_not_norm_);
}
