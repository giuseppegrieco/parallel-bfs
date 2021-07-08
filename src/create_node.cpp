#include "graph_utils.h"

void create_node(int id, int label, vector<node> &nodes, map<int, int> &nodePositions) {
    node newNode;
    newNode.label = label;
    newNode.visited = false;
    if (nodePositions.find(id) == nodePositions.end()) {
        nodePositions[id] = nodes.size();
        nodes.push_back(newNode);
    }
}
