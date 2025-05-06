 #ifndef INFOSET_H
#define INFOSET_H

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

    InfoSet(int n_actions);
    InfoSet(const InfoSet& other);
    
    const vector<double>& getInstantRegrets() const;
    const vector<double>& getRegretSum() const;
    const vector<double>& getRegretSumStrategy();
    vector<double> getCumulativeStrategy();

    virtual void setInstantRegret(
        int action_idx, double regret
    );

    virtual void accumulateRegrets(double weight);
    void accumulateStrategy(double weight);

protected:
    vector<double> instant_regrets;
    vector<double> regret_sum;
    bool regret_sum_strategy_updated;
    vector<double> regret_sum_strategy;
    vector<double> cumulative_strategy_raw;

    vector<double> normalizeStrategy(const vector<double>& strategy);
    vector<double> epsilonSoftStrategy(double epsilon, const vector<double>& strategy);
};


class InfoSetNonnegativeRegret : public InfoSet {
    using InfoSet::InfoSet;

public:
    virtual void accumulateRegrets(double weight) override;
};

#endif  // INFOSET_H