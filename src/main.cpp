#include <iostream>
#include <random>

#include "bfs_search.h"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cerr << "Usage: ./bfs inputFile \n";
        return -1;
    }
    string inputFile = argv[1];

    /**
    Generates random dag

    vector<node> nodes = generate_random_graph(20, 50);
    save_graph(nodes, "examples/1.dat");
    */

    pair<int, vector<node>> run = load_run_from_file(inputFile);

    int target = run.first;
    cout << "Node target: " << target << endl;
    vector<node> nodes = run.second;


    bool found = bfs_search_thread(target, nodes, 4);

    if(!found) {
        cout << "Node target not found" << endl;
    }

    return 0;
}
