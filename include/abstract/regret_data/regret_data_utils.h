#pragma once

#include "abstract/regret_data/regret_data.h"
#include "abstract/regret_data/regret_data_map.h"

using namespace std;


namespace regret_data_utils {
    template <typename KeyFrom, typename KeyTo>
    KeyTo convertKey(const KeyFrom& key) = delete;

    // Implementation
    template <>
    inline size_t convertKey<string, size_t>(const string& key) {
        return std::hash<string>{}(key);
    }

    template <>
    inline string convertKey<size_t, string>(const size_t& key) {
        return to_string(key);
    }

    template <>
    inline string convertKey<string, string>(const string& key) {
        return key;
    }

    template <>
    inline size_t convertKey<size_t, size_t>(const size_t& key) {
        return key;
    }


    class SaveLoader {
    public:
        // Helper function to save InfoSet data using a data extractor function
        template <RegretDataKey Key>
        static void saveInfoSetMapToFile(
            const RegretDataMap<Key>& infoset_map,
            const string& filename,
            vector<double> RegretData::*extract_data_prop
        );
        
        // Helper function to load InfoSet data using a data applier function
        template <RegretDataKey Key>
        static void loadInfoSetMapDataFromFile(
            RegretDataMap<Key>& map,
            const string& filename,
            vector<double> RegretData::*apply_data_prop
        );

        template <RegretDataKey Key>
        static void saveInfoSetMapStrategy(
            const RegretDataMap<Key>& infoset_map, 
            const string& filename
        );

        template <RegretDataKey Key>
        static void saveInfoSetMapRegretSum(
            const RegretDataMap<Key>& infoset_map, 
            const string& filename
        );

        template <RegretDataKey Key>
        static void saveInfoSetMapInstantRegret(
            const RegretDataMap<Key>& infoset_map, 
            const string& filename
        );

        template <RegretDataKey Key>
        static RegretDataMap<Key> loadInfoSetMap(
            const string& regretsum_file,
            const string& cumulative_strategy_file
        );
    };

    class Converter {
    public:
        template <RegretDataKey KeyTo, RegretDataKey KeyFrom>
        static RegretDataMap<KeyTo> convertTo(const RegretDataMap<KeyFrom>& infoset_map);
    };
    
    struct RegretMetric {
        int n_regrets;
        double max_instant_regret;
        double sum_positive_instant_regrets;
    };
    
    template <RegretDataKey Key>
    RegretMetric calculateMetric(const RegretDataMap<Key>& infoset_map);
};

#include "abstract/regret_data/regret_data_utils.hpp"