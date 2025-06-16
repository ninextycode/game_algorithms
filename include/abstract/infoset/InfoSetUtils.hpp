#pragma once

#include "abstract/infoset/InfoSetUtils.h"
#include <fstream>

template <InfoSetKey Key>
void infoset_utils::SaveLoader::saveInfoSetMapToFile(
    const InfoSetMap<Key>& infoset_map,
    const string& filename,
    vector<double> InfoSet::* extract_data_prop
) {
    ofstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    file << "{" << endl;
    
    bool firstEntry = true;
    for (const auto& entry : infoset_map) {
        if (!firstEntry) {
            file << "," << endl;
        }
        firstEntry = false;

        const auto& key = entry.first;
        const auto& infoset = entry.second;
        
        // Escape the key for JSON
        string escaped_key = convertKey<Key, string>(key);
        size_t pos = 0;

        // Replace newlines with escaped newlines
        pos = 0;
        while ((pos = escaped_key.find("\n", pos)) != string::npos) {
            escaped_key.replace(pos, 1, "\\n");
            pos += 2;
        }
        
        file << "  \"" << escaped_key << "\": [";
        
        // use pointer to property to extract data
        const auto& values = infoset.*extract_data_prop;
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) {
                file << ", ";
            }
            file << values[i];
        }
        
        file << "]";
    }
    
    file << endl << "}" << endl;
    file.close();
}



// Now implement the strategy save/load functions using the helper functions
template <InfoSetKey Key>
void infoset_utils::SaveLoader::saveInfoSetMapStrategy(
    const InfoSetMap<Key>& infoset_map, 
    const string& filename
) {
    saveInfoSetMapToFile(
        infoset_map, filename, 
        &InfoSet::cumulative_strategy_not_norm_
    );
}

// Implement the regret sum save/load functions
template <InfoSetKey Key>
void infoset_utils::SaveLoader::saveInfoSetMapRegretSum(
    const InfoSetMap<Key>& infoset_map, 
    const string& filename
) {
    saveInfoSetMapToFile(
        infoset_map, filename, 
        &InfoSet::regret_sum_
    );
}

// Implement the instant regret save/load functions
template <InfoSetKey Key>
void infoset_utils::SaveLoader::saveInfoSetMapInstantRegret(
    const InfoSetMap<Key>& infoset_map, 
    const string& filename
) {
    saveInfoSetMapToFile(
        infoset_map, filename, 
        &InfoSet::instant_regret_
    );
}


// Helper function to load InfoSet data using a data applier function
template <InfoSetKey Key>
void infoset_utils::SaveLoader::loadInfoSetMapDataFromFile(
    InfoSetMap<Key>& infoset_map,
    const string& filename,
    vector<double> InfoSet::* apply_data_prop
) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }

    // Read the entire file into a string
    string json_str((istreambuf_iterator<char>(file)),
                         istreambuf_iterator<char>());
    file.close();

    // Simple JSON parsing - assuming well-formed JSON with the expected format
    size_t pos = json_str.find('{');
    if (pos == string::npos) {
        throw runtime_error("Invalid JSON format: missing opening brace");
    }
    
    size_t end_pos = json_str.rfind('}');
    if (end_pos == string::npos) {
        throw runtime_error("Invalid JSON format: missing closing brace");
    }
    
    // Extract content between braces
    string content = json_str.substr(pos + 1, end_pos - pos - 1);
    
    // Parse each info set entry
    pos = 0;
    while (pos < content.length()) {
        // Find the key
        size_t key_start = content.find('"', pos);
        if (key_start == string::npos) break;
        
        size_t key_end = content.find('"', key_start + 1);
        if (key_end == string::npos) break;
        
        // Extract and unescape the key
        string key_str = content.substr(key_start + 1, key_end - key_start - 1);
        // Replace escaped newlines with actual newlines
        size_t escape_pos = 0;
        while ((escape_pos = key_str.find("\\n", escape_pos)) != string::npos) {
            key_str.replace(escape_pos, 2, "\n");
            escape_pos += 1;
        }
        
        // Find the array start
        size_t array_start = content.find('[', key_end);
        if (array_start == string::npos) break;
        
        size_t array_end = content.find(']', array_start);
        if (array_end == string::npos) break;
        
        // Extract the array content
        string array_content = content.substr(array_start + 1, array_end - array_start - 1);
        
        // Parse the array values
        vector<double> values;
        size_t value_pos = 0;
        while (value_pos < array_content.length()) {
            // Skip whitespace and commas
            while (value_pos < array_content.length() && 
                   (isspace(array_content[value_pos]) || array_content[value_pos] == ',')) {
                value_pos++;
            }
            if (value_pos >= array_content.length()) break;
            
            // Find the end of the number
            size_t number_end = value_pos;
            while (number_end < array_content.length() && 
                   (isdigit(array_content[number_end]) || array_content[number_end] == '.' || 
                    array_content[number_end] == '-' || array_content[number_end] == 'e' || 
                    array_content[number_end] == 'E' || array_content[number_end] == '+')) {
                number_end++;
            }
            
            if (number_end > value_pos) {
                string number_str = array_content.substr(value_pos, number_end - value_pos);
                double value = stod(number_str);
                values.push_back(value);
            }
            
            value_pos = number_end;
        }
        
        // Check if InfoSet already exists, create if not
        Key key = convertKey<string, Key>(key_str);
        auto it = infoset_map.find(key);
        
        if (it == infoset_map.end()) {
            // Create new InfoSet with the parsed values
            InfoSet infoset(values.size());
            infoset.*apply_data_prop = std::move(values);
            infoset_map.try_emplace(key, std::move(infoset));
        } else {
            // Update existing InfoSet with the loaded values
            it->second.*apply_data_prop = std::move(values);
        }
        
        // Move to the next entry
        pos = array_end + 1;
    }
}


template <InfoSetKey Key>
InfoSetMap<Key> infoset_utils::SaveLoader::loadInfoSetMap(
    const string& regretsum_file,
    const string& cumulative_strategy_file
) {
    // Load regret sums first
    InfoSetMap<Key> infoset_map;
    loadInfoSetMapDataFromFile(
        infoset_map,
        regretsum_file,
        &InfoSet::regret_sum_
    );
    
    loadInfoSetMapDataFromFile(
        infoset_map,
        cumulative_strategy_file,
        &InfoSet::cumulative_strategy_not_norm_
    );
    

    // Reset the strategy update flag for all InfoSets
    for (auto& [key, infoset] : infoset_map) {
        infoset.regret_sum_strategy_uptodate_ = false;
        infoset.cumulative_strategy_uptodate_ = false;
    }

    return infoset_map;
}



template <InfoSetKey KeyTo, InfoSetKey KeyFrom>
InfoSetMap<KeyTo> infoset_utils::Converter::convertTo(
    const InfoSetMap<KeyFrom>& infoset_map
) {
    InfoSetMap<KeyTo> converted_map;
    
    for (const auto& [key, infoset] : infoset_map) {
        KeyTo converted_key = convertKey<KeyFrom, KeyTo>(key);
        converted_map.try_emplace(converted_key, infoset);
    }
    
    return converted_map;
}



template <InfoSetKey Key>
infoset_utils::RegretMetric infoset_utils::calculateMetric(
    const InfoSetMap<Key>& infoset_map
) {
    RegretMetric metric = {0, 0.0, 0.0};

    for (const auto& pair : infoset_map) {
        const auto& infoset = pair.second;
        const vector<double>& regrets = infoset.getInstantRegret();
        
        for (const double& regret : regrets) {
            metric.n_regrets++;
            
            if (regret > metric.max_instant_regret) {
                metric.max_instant_regret = regret;
            }
            
            if (regret > 0) {
                metric.sum_positive_instant_regrets += regret;
            }
        }
    }

    return metric;
}