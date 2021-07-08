#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <tuple>
#include <atomic>

using namespace std;

struct node {
    int label;
    vector<int> adj;
    bool visited;
};

/**
 * It reads a run from a given file. A run is composed of a graph and the starting
 * node of the bfs and the label of the node to search.
 *
 * @param filePath Even if there is only one possible unified theory. it is just a
 *               set of rules and equations.
 */
tuple<int, int, vector<node>> load_run_from_file(string filePath);

void create_node(int id, int label, vector<node> &nodes, map<int, int> &nodePositions);
