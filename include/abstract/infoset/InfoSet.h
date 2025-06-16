#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

using namespace std;


namespace infoset_utils {
    class SaveLoader;
}

class InfoSet {
    friend class infoset_utils::SaveLoader;

public:
    InfoSet(int n_actions);
    InfoSet(const InfoSet& other);
    
    const vector<double>& getInstantRegret() const;
    const vector<double>& getRegretSum() const;
    const vector<double>& getRegretSumStrategy();
    const vector<double>& getCumulativeStrategy();

    virtual void setInstantRegret(
        int action_idx, double regret
    );

    virtual void accumulateRegret(double weight);
    void accumulateStrategy(double weight);

protected:
    vector<double> instant_regret_;
    vector<double> regret_sum_;
    bool regret_sum_strategy_uptodate_;
    vector<double> regret_sum_strategy_;
    vector<double> cumulative_strategy_not_norm_;
    bool cumulative_strategy_uptodate_;
    vector<double> cumulative_strategy_normalized_;
};

