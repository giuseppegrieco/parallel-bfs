#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

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
pair<int, vector<node>> load_run_from_file(string filePath);

vector<node> generate_random_graph(int nLevels, int maxNodePerLevel);

void save_graph(vector<node> nodes, string outputPath);

void create_node(int label, vector<node> &nodes, map<int, int> &nodePositions);
