#pragma once

#include "abstract/infoset/InfoSet.h"
#include "abstract/infoset/InfoSetMap.h"

using namespace std;


namespace infoset_utils {
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
        template <InfoSetKey Key>
        static void saveInfoSetMapToFile(
            const InfoSetMap<Key>& infoset_map,
            const string& filename,
            vector<double> InfoSet::*extract_data_prop
        );
        
        // Helper function to load InfoSet data using a data applier function
        template <InfoSetKey Key>
        static void loadInfoSetMapDataFromFile(
            InfoSetMap<Key>& map,
            const string& filename,
            vector<double> InfoSet::*apply_data_prop
        );

        template <InfoSetKey Key>
        static void saveInfoSetMapStrategy(
            const InfoSetMap<Key>& infoset_map, 
            const string& filename
        );

        template <InfoSetKey Key>
        static void saveInfoSetMapRegretSum(
            const InfoSetMap<Key>& infoset_map, 
            const string& filename
        );

        template <InfoSetKey Key>
        static void saveInfoSetMapInstantRegret(
            const InfoSetMap<Key>& infoset_map, 
            const string& filename
        );

        template <InfoSetKey Key>
        static InfoSetMap<Key> loadInfoSetMap(
            const string& regretsum_file,
            const string& cumulative_strategy_file
        );
    };

    class Converter {
    public:
        template <InfoSetKey KeyTo, InfoSetKey KeyFrom>
        static InfoSetMap<KeyTo> convertTo(const InfoSetMap<KeyFrom>& infoset_map);
    };
    
    struct RegretMetric {
        int n_regrets;
        double max_instant_regret;
        double sum_positive_instant_regrets;
    };
    
    template <InfoSetKey Key>
    RegretMetric calculateMetric(const InfoSetMap<Key>& infoset_map);
};

#include "abstract/infoset/InfoSetUtils.hpp"