#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

using namespace std;

class InfoSet;

using InfoSetMap = unordered_map<string, shared_ptr<InfoSet>>;

class InfoSet {
public:
    static void saveInfoSetMapStrategy(
        const InfoSetMap& infoset_map, 
        const string& filename
    );

    static void saveInfoSetMapRegretSum(
        const InfoSetMap& infoset_map, 
        const string& filename
    );

    static void saveInfoSetMapInstantRegret(
        const InfoSetMap& infoset_map, 
        const string& filename
    );

    static InfoSetMap loadInfoSetMap(
        const string& regretsum_file,
        const string& cumulative_strategy_file
    );

    InfoSet(int n_actions);
    InfoSet(const InfoSet& other);
    
    const vector<double>& getInstantRegret() const;
    const vector<double>& getRegretSum() const;
    vector<double> getRegretSumStrategy(double e_soft=0);
    vector<double> getCumulativeStrategy();

    virtual void setInstantRegret(
        int action_idx, double regret
    );

    virtual void accumulateRegret(double weight);
    void accumulateStrategy(double weight, double e_soft=0);

protected:
    vector<double> instant_regret_;
    vector<double> regret_sum_;
    bool regret_sum_hard_strategy_updated_;
    vector<double> regret_sum_hard_strategy_;
    vector<double> cumulative_strategy_raw_;

    vector<double> normalizeStrategy(const vector<double>& strategy);
    vector<double> epsilonSoftStrategy(double epsilon, const vector<double>& strategy);
};