#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <utility>
#include <string>
#include "abstract/nodes/GameNode.h"

using namespace std;

/**
 * @class GraphTraversal
 * @brief Performs breadth-first traversal of a game tree starting from a root node.
 * 
 * This class traverses a game tree in breadth-first order, storing all visited nodes
 * and maintaining parent-child relationships for efficient navigation.
 */
class GraphTraversal {
public:
    using ActionChildPair = pair<int, size_t>;
    using ParentActionPair = pair<size_t, int>;

    /**
     * @brief Constructs a GraphTraversal object and performs traversal.
     * @param root The root node to start traversal from
     */
    explicit GraphTraversal(shared_ptr<const GameNode> root);

    /**
     * @brief Gets the number of nodes visited during traversal.
     * @return Number of nodes in the tree
     */
    size_t getNodeCount() const;

    /**
     * @brief Exports the graph structure to a file in JSON format.
     * @param filename The path to the output file
     */
    void exportGraph(const string& filename) const;

private:
    vector<shared_ptr<const GameNode>> nodes_;
    queue<size_t> node_queue_;
    unordered_map<size_t, vector<ActionChildPair>> parent_to_children_;
    unordered_map<size_t, ParentActionPair> child_to_parent_;
    static const vector<ActionChildPair> empty_children_;
    static const ParentActionPair empty_parent_;

    void performTraversal(shared_ptr<const GameNode> root);
    void processNextQueuedNode();

    string createNodeString(size_t node_idx) const;
    string createEdgeString(
        size_t parent_idx, 
        int action, 
        size_t child_idx
    ) const;
};