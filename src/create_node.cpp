#include "graph_utils.h"

void create_node(int label, vector<node> &nodes, map<int, int> &nodePositions) {
    node newNode;
    newNode.label = label;
    newNode.visited = false;
    if (nodePositions.find(label) == nodePositions.end()) {
        nodePositions[label] = nodes.size();
        nodes.push_back(newNode);
    }
}
