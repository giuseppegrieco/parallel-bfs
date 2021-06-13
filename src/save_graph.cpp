#include "graph_utils.h"

void save_graph(vector<node> nodes, string outputPath) {
    fstream outputFile(outputPath, ios_base::out);
    for(size_t i = 0; i < nodes.size(); i++) {
        for(size_t j = 0; j < nodes[i].adj.size(); j++) {
            string in = to_string(nodes[i].label);
            string out = to_string(nodes[nodes[i].adj[j]].label);
            string line = in + "," + out;
            outputFile << line << endl;
        }
    }
    outputFile.close();
}
