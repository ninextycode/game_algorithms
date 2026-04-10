#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <queue>
#include <utility>
#include <string>
#include <stdexcept>
#include "abstract/nodes/game_node.h"

using namespace std;

/**
 * @class GraphTraversal
 * @brief Performs breadth-first traversal of a game tree starting from a root node.
 * 
 * This class traverses a game tree in breadth-first order, storing all visited nodes
 * and maintaining parent-child relationships for efficient navigation.
 */
template<typename T>
requires is_base_of_v<TypedGameNode<T>, T>
class GraphTraversal {
public:
    using ActionChildPair = pair<int, size_t>;
    using ParentActionPair = pair<size_t, int>;

    /**
     * @brief Constructs a GraphTraversal object and performs traversal.
     * @param root The root node to start traversal from
     * @param depth_limit Maximum depth to traverse (-1 for no limit)
     */
    explicit GraphTraversal(shared_ptr<const T> root, int depth_limit = -1);

    /**
     * @brief Gets the number of nodes visited during traversal.
     * @return Number of nodes in the tree
     */
    size_t getNodeCount() const;

    /**
     * @brief Gets a node by its ID.
     * @param node_id The ID of the node to retrieve
     * @return Shared pointer to the node
     * @throws out_of_range if node_id is invalid
     */
    shared_ptr<const T> getNode(size_t node_id) const;

    /**
     * @brief Gets the child node ID for a given parent and action.
     * @param parent_id The ID of the parent node
     * @param action The action taken from the parent
     * @return The ID of the child node
     * @throws out_of_range if parent_id is invalid or action doesn't exist
     */
    size_t getChildId(size_t parent_id, int action) const;

    /**
     * @brief Gets the parent node ID for a given child.
     * @param child_id The ID of the child node
     * @return The ID of the parent node
     * @throws out_of_range if child_id is invalid or is the root node
     */
    size_t getParentId(size_t child_id) const;

    /**
     * @brief Gets the parent node ID and action for a given child.
     * @param child_id The ID of the child node
     * @return Pair of (parent_id, action)
     * @throws out_of_range if child_id is invalid or is the root node
     */
    ParentActionPair getParentAndAction(size_t child_id) const;

    /**
     * @brief Gets all children IDs for a given parent.
     * @param parent_id The ID of the parent node
     * @return Vector of (action, child_id) pairs
     * @throws out_of_range if parent_id is invalid
     */
    const vector<ActionChildPair>& getChildren(size_t parent_id) const;

    /**
     * @brief Exports the graph structure to a file in JSON format.
     * @param filename The path to the output file
     */
    void exportGraph(const string& filename) const;

private:
    vector<shared_ptr<const T>> nodes_;
    queue<pair<size_t, int>> node_queue_;  // (node_idx, depth)
    unordered_map<size_t, vector<ActionChildPair>> parent_to_children_;
    unordered_map<size_t, ParentActionPair> child_to_parent_;
    static const vector<ActionChildPair> empty_children_;
    int depth_limit_;

    void performTraversal(shared_ptr<const T> root);
    void processNextQueuedNode();

    string createNodeString(size_t node_idx) const;
    string createEdgeString(
        size_t parent_idx, 
        int action, 
        size_t child_idx
    ) const;
    
    static string escape(const string& str);
};

#include "traversal/graph_traversal.hpp"
