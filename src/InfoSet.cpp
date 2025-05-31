#include "InfoSet.h"
#include <fstream>
#include <functional>

// Helper function to save InfoSet data using a data extractor function
void saveInfoSetMapToFile(
    const InfoSetMap& infoset_map,
    const string& filename,
    std::function<vector<double>(const shared_ptr<InfoSet>&)> extractData
) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    file << "{" << std::endl;
    
    bool firstEntry = true;
    for (const auto& entry : infoset_map) {
        if (!firstEntry) {
            file << "," << std::endl;
        }
        firstEntry = false;

        const auto& key = entry.first;
        const auto& infoset = entry.second;
        
        // Escape the key for JSON
        std::string escaped_key = key;
        size_t pos = 0;

        // Replace newlines with escaped newlines
        pos = 0;
        while ((pos = escaped_key.find("\n", pos)) != std::string::npos) {
            escaped_key.replace(pos, 1, "\\n");
            pos += 2;
        }
        
        file << "  \"" << escaped_key << "\": [";
        
        const auto& values = extractData(infoset);
        for (size_t i = 0; i < values.size(); ++i) {
            if (i > 0) {
                file << ", ";
            }
            file << values[i];
        }
        
        file << "]";
    }
    
    file << std::endl << "}" << std::endl;
    file.close();
}

// Helper function to load InfoSet data using a data applier function
InfoSetMap loadInfoSetMapFromFile(
    const string& filename,
    std::function<void(const shared_ptr<InfoSet>&, const vector<double>&)> applyData
) {
    InfoSetMap infoset_map;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    // Read the entire file into a string
    std::string json_str((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    file.close();

    // Simple JSON parsing - assuming well-formed JSON with the expected format
    size_t pos = json_str.find('{');
    if (pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format: missing opening brace");
    }
    
    size_t end_pos = json_str.rfind('}');
    if (end_pos == std::string::npos) {
        throw std::runtime_error("Invalid JSON format: missing closing brace");
    }
    
    // Extract content between braces
    std::string content = json_str.substr(pos + 1, end_pos - pos - 1);
    
    // Parse each info set entry
    pos = 0;
    while (pos < content.length()) {
        // Find the key
        size_t key_start = content.find('"', pos);
        if (key_start == std::string::npos) break;
        
        size_t key_end = content.find('"', key_start + 1);
        if (key_end == std::string::npos) break;
        
        // Extract and unescape the key
        std::string key = content.substr(key_start + 1, key_end - key_start - 1);
        // Replace escaped newlines with actual newlines
        size_t escape_pos = 0;
        while ((escape_pos = key.find("\\n", escape_pos)) != std::string::npos) {
            key.replace(escape_pos, 2, "\n");
            escape_pos += 1;
        }
        
        // Find the array start
        size_t array_start = content.find('[', key_end);
        if (array_start == std::string::npos) break;
        
        size_t array_end = content.find(']', array_start);
        if (array_end == std::string::npos) break;
        
        // Extract the array content
        std::string array_content = content.substr(array_start + 1, array_end - array_start - 1);
        
        // Parse the array values
        std::vector<double> values;
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
                std::string number_str = array_content.substr(value_pos, number_end - value_pos);
                double value = std::stod(number_str);
                values.push_back(value);
            }
            
            value_pos = number_end;
        }
        
        // Create InfoSet with the parsed values
        auto infoset = std::make_shared<InfoSet>(values.size());
        
        // Apply the loaded values using the provided function
        applyData(infoset, values);
        
        // Add to map
        infoset_map[key] = infoset;
        
        // Move to the next entry
        pos = array_end + 1;
    }
    
    return infoset_map;
}

// Now implement the strategy save/load functions using the helper functions
void InfoSet::saveInfoSetMapStrategy(
    const InfoSetMap& infoset_map, 
    const string& filename
) {
    saveInfoSetMapToFile(infoset_map, filename, 
        [](const shared_ptr<InfoSet>& infoset) {
            return infoset->getCumulativeStrategy();
        }
    );
}

// Implement the regret sum save/load functions
void InfoSet::saveInfoSetMapRegretSum(
    const InfoSetMap& infoset_map, 
    const string& filename
) {
    saveInfoSetMapToFile(infoset_map, filename, 
        [](const shared_ptr<InfoSet>& infoset) {
            return infoset->getRegretSum();
        }
    );
}

// Implement the instant regret save/load functions
void InfoSet::saveInfoSetMapInstantRegret(
    const InfoSetMap& infoset_map, 
    const string& filename
) {
    saveInfoSetMapToFile(infoset_map, filename, 
        [](const shared_ptr<InfoSet>& infoset) {
            return infoset->getInstantRegret();
        }
    );
}

InfoSetMap InfoSet::loadInfoSetMap(
    const string& regretsum_file,
    const string& cumulative_strategy_file
) {
    // TODO - not the cleanest approach with merging

    // Load regret sums first
    InfoSetMap infoset_map = loadInfoSetMapFromFile(regretsum_file, 
        [](const shared_ptr<InfoSet>& infoset, const vector<double>& values) {
            for (size_t i = 0; i < values.size(); ++i) {
                infoset->regret_sum_[i] = values[i];
            }
            infoset->regret_sum_hard_strategy_updated_ = false;
        }
    );
    
    // Load cumulative strategies and merge with existing InfoSets
    InfoSetMap strategy_map = loadInfoSetMapFromFile(cumulative_strategy_file,
        [](const shared_ptr<InfoSet>& infoset, const vector<double>& values) {
            for (size_t i = 0; i < values.size(); ++i) {
                infoset->cumulative_strategy_raw_[i] = values[i];
            }
        }
    );
    
    // Merge strategy data into the main map
    for (const auto& entry : strategy_map) {
        const string& key = entry.first;
        const auto& strategy_infoset = entry.second;
        
        if (infoset_map.find(key) != infoset_map.end()) {
            // InfoSet exists, copy strategy data
            auto& existing_infoset = infoset_map[key];
            existing_infoset->cumulative_strategy_raw_ = strategy_infoset->cumulative_strategy_raw_;
        } else {
            // InfoSet doesn't exist, add it
            infoset_map[key] = strategy_infoset;
        }
    }
    
    return infoset_map;
}


InfoSet::InfoSet(int n_actions) 
    : instant_regret_(n_actions, 0.0),
      regret_sum_(n_actions, 20.0), // TODO return to 0
      regret_sum_hard_strategy_updated_(false),
      regret_sum_hard_strategy_(n_actions, 0.0),
      cumulative_strategy_raw_(n_actions, 0.0) {
}

InfoSet::InfoSet(const InfoSet& other)
    : instant_regret_(other.instant_regret_),
      regret_sum_(other.regret_sum_),
      regret_sum_hard_strategy_updated_(other.regret_sum_hard_strategy_updated_),
      regret_sum_hard_strategy_(other.regret_sum_hard_strategy_),
      cumulative_strategy_raw_(other.cumulative_strategy_raw_) {
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
    regret_sum_hard_strategy_updated_ = false;
}

void InfoSet::accumulateStrategy(double weight, double e_soft) {
    const vector<double>& strategy = getRegretSumStrategy(e_soft);
    for (size_t i = 0; i < strategy.size(); i++) {
        cumulative_strategy_raw_[i] += weight * strategy[i];
    }
}

const vector<double>& InfoSet::getInstantRegret() const {
    return instant_regret_;
}

const vector<double>& InfoSet::getRegretSum() const {
    return regret_sum_;
}

// Static helper function to normalize strategies
vector<double> InfoSet::normalizeStrategy(const vector<double>& strategy) {
    vector<double> normalized = strategy;

    for (size_t i = 0; i < normalized.size(); i++) {
        normalized[i] = max(0.0, normalized[i]);
    }

    double sum = 0.0;
    for (size_t i = 0; i < normalized.size(); i++) {
        sum += normalized[i];
    }
    
    // If sum is positive, normalize the strategy
    if (sum > 0) {
        for (size_t i = 0; i < normalized.size(); i++) {
            normalized[i] /= sum;
        }
    } else {
        // Use uniform strategy if sum is zero
        double uniformProb = 1.0 / normalized.size();
        std::fill(normalized.begin(), normalized.end(), uniformProb);
    }
    
    return normalized;
}


vector<double> InfoSet::epsilonSoftStrategy(
    double epsilon, const vector<double>& strategy
) {
    vector<double> softened_strategy = strategy;
    size_t n = strategy.size();
    
    // Reserve at least epsilon for all random actions
    double remaining_prob = 1.0 - epsilon;
    
    for (size_t i = 0; i < n; ++i) {
        // Each action gets epsilon / n + proportional share of remaining probability
        softened_strategy[i] = epsilon / n + (remaining_prob * strategy[i]);
    }
    
    return softened_strategy;
}

vector<double> InfoSet::getRegretSumStrategy(double e_soft) {
    if (!regret_sum_hard_strategy_updated_) {
        // Normalize the strategy - this step can be precomputed
        regret_sum_hard_strategy_ = normalizeStrategy(regret_sum_);
        regret_sum_hard_strategy_updated_ = true;
    }

    // soften strategy - experimental
    // different coefficients are possible, so have to re-compute every time
    vector<double> regret_sum_strategy;
    if (e_soft > 0) {
        regret_sum_strategy = epsilonSoftStrategy(e_soft, regret_sum_hard_strategy_);
    } else {
        regret_sum_strategy = regret_sum_hard_strategy_;
    }

    return regret_sum_strategy;
}

vector<double> InfoSet::getCumulativeStrategy() {
    // Create a copy, normalize it, and update the original
    vector<double> cumulative_strategy_normalized =\
        normalizeStrategy(cumulative_strategy_raw_);
    return cumulative_strategy_normalized;
}
