#pragma once

#include <string>
#include <vector>


using namespace std;

namespace cpp_utils {
    string demangle(const char* mangled);
}

namespace strategy_utils {
    vector<double> normalizeStrategy(
        const vector<double>& strategy
    );

    // Each action gets at least epsilon / n probability
    // This is equivalent to, with probability "epsilon", selecting a random action uniformly
    // Or sticking with an existing strategy with probability "1 - epsilon"
    vector<double> epsilonSoftStrategy(
        double epsilon, const vector<double>& strategy
    );
}
