#include <iostream>
#include <memory>
#include "cfr/CFRPlus.h"
#include "tictactoe/TTTInvariant.h"
#include "abstract/nodes/Randomizer.h"
#include "abstract/infoset/InfoSetUtils.h"
#include <chrono>

using namespace std;

void training(int n_steps) {
    shared_ptr<GameNode> ttt_inv = make_shared<TTTInvariant>();
    // shared_ptr<GameNode> ttt_inv_rand = make_shared<RandomizerWrapNode>(
    //     ttt_inv, 0.2, max_random_actions
    // );
    

    InfoSetMap<string> map_str = infoset_utils::SaveLoader::loadInfoSetMap<string>(
        "training_output/tic_tac_toe_cpp_regretsum.json",
        "training_output/tic_tac_toe_cpp_strategy.json"
    );

    cout << "CFRPlus<> constructor started" << endl;
    auto start = chrono::high_resolution_clock::now();
    CFRPlus<> cfr = CFRPlus<>::Builder()
        .setRootNode(ttt_inv)
        .setInitialState(map_str)
        .buildCfr();

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "CFRPlus<> constructor took: " << duration.count() / 1000.0 << " seconds" << endl;
    cout << "test" << endl;


    for (int i = 0; i < n_steps; i++) {
        cout << "step " << i + 1 << " of " << n_steps << endl;
        // dont accumulate strategy until half of the training steps
        cfr.evaluateAndUpdate(true, i > n_steps / 2);
        auto& step_info_sets = cfr.getStrategyInfoSets();
        auto metric = infoset_utils::calculateMetric(step_info_sets);
        cout << "sum_positive_instant_regrets: " 
             << metric.sum_positive_instant_regrets << endl;
    }
    cout << "CFRPlus<> evaluation completed" << endl;

    auto& strategy_info_sets = cfr.getStrategyInfoSets();

    // Save the strategy to files
    infoset_utils::SaveLoader::saveInfoSetMapRegretSum<>(
        strategy_info_sets, "training_output/tic_tac_toe_cpp_regretsum.json"
    );
    infoset_utils::SaveLoader::saveInfoSetMapStrategy<>(
        strategy_info_sets, "training_output/tic_tac_toe_cpp_strategy.json"
    );
}


void read_results() {
    auto map_str = infoset_utils::SaveLoader::loadInfoSetMap<string>(
        "training_output/tic_tac_toe_cpp_regretsum.json",
        "training_output/tic_tac_toe_cpp_strategy.json"
    );


    auto empty_board = TicTacToeBoard(array<int, 9>({-1, -1, -1, -1, -1, -1, -1, -1, -1}));
    auto x_mid_board = TicTacToeBoard(array<int, 9>({-1, -1, -1, -1, 0, -1, -1, -1, -1}));
    auto x_corner_board = TicTacToeBoard(array<int, 9>({0, -1, -1, -1, -1, -1, -1, -1, -1}));
    auto x_side_board = TicTacToeBoard(array<int, 9>({-1, 0, -1, -1, -1, -1, -1, -1, -1}));
    auto x_towin_board = TicTacToeBoard(array<int, 9>({
        -1,  -1, -1,
        -1,  0, -1,
        -1,  1, -1
    }));

    vector<string> board_names = {
        "empty_board",
        "x_mid_board",
        "x_corner_board",
        "x_side_board",
        "x_towin_board"
    };
    vector<TicTacToeBoard> boards = {
        empty_board,
        x_mid_board,
        x_corner_board,
        x_side_board,
        x_towin_board
    };

    for (size_t i = 0; i < boards.size(); i++) {
        cout << "Board: " << board_names[i] << endl;
        cout << "String representation: \n" << boards[i].getNormalForm().toString() << endl;
        shared_ptr<TTTInvariant> node = make_shared<TTTInvariant>(
            TicTacToeNode(boards[i].getNormalForm())
        );

        vector<double> strat_prob = \
            map_str.at(node->getInfoSetKeyString()).getCumulativeStrategy();
        vector<int> actions = node->getLegalActions();

        cout << "Legal actions: ";
        for (size_t j = 0; j < actions.size(); j++) {
            cout << actions[j] << "(p=" << strat_prob[j] << ") ";
        }
        cout << endl;

        double x_board_value = \
            CFRPlus<>::Builder()
                .setInitialState(map_str)
                .setInitialEvaluationRun(false)
                .setRootNode(node)
                // .setESoftRegsumStrategies(0.2)
                .buildCfr()
                .evaluate();
        
        cout << "Value X player: " << x_board_value << endl;
        cout << endl;
    }
}



int main(int argc, char** argv) {
    int n_steps = 10;
    if (argc > 1) {
        n_steps = stoi(argv[1]);
    }

    int max_random_actions = -1;
    if (argc > 2) {
        max_random_actions = atoi(argv[2]);
    }

    // training(n_steps);
    read_results();

    return 0;
}