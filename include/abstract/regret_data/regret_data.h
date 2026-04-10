#pragma once

#include <vector>

using namespace std;


namespace regret_data_utils {
    class SaveLoader;
}

class RegretData {
    friend class regret_data_utils::SaveLoader;

public:
    RegretData(
        const vector<double>& regret_sum,
        const vector<double>& cumulative_strategy_not_norm
    );
    RegretData(int n_actions);
    RegretData(const RegretData& other);
    
    const vector<double>& getInstantRegret() const;
    const vector<double>& getRegretSum() const;
    const vector<double>& getRegretSumStrategy();
    const vector<double>& getCumulativeStrategy();
    vector<double> getRegretSumStrategy() const;
    vector<double> getCumulativeStrategy() const;

    virtual void setInstantRegret(
        int action_idx, double regret
    );

    virtual void accumulateRegret(double weight);
    void accumulateStrategy(double weight);

    const vector<double>& getCumulativeStrategyNotNorm() const;
protected:
    vector<double> instant_regret_;
    vector<double> regret_sum_;
    bool regret_sum_strategy_uptodate_;
    vector<double> regret_sum_strategy_;
    vector<double> cumulative_strategy_not_norm_;
    bool cumulative_strategy_uptodate_;
    vector<double> cumulative_strategy_normalized_;
};

