#include <iostream>
#include <random>

#include "bfs_search.h"
#include "utimer.h"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cerr << "Usage: ./bfs inputFile runMode [nw] \n";
        return -1;
    }
    string inputFile = argv[1];
    int runMode = 0;
    int nw = -1;
    if(argc > 2) {
        runMode = atoi(argv[2]);
    }
    if(runMode > 0 && argc < 4) {
        cerr << "Usage: ./bfs inputFile runMode nw \n";
        nw = atoi(argv[3]);
        return -1;
    }

    cout << runMode << endl;

    Generates random dag

    vector<node> nodes = generate_random_graph(10, 20);
    save_graph(nodes, "examples/1.dat");

    pair<int, vector<node>> run = load_run_from_file(inputFile);

    int target = run.first;
    cout << "Node target: " << target << endl;
    vector<node> nodes = run.second;

    bool found = false;
    utimer u("bfs");
    switch(runMode) {
        case 0:
            cout << "Sequential version" << endl;
            found = bfs_search_seq(target, nodes);
            break;
        case 1:
            cout << "Thread version" << endl;
            found = bfs_search_thread(target, nodes, nw);
            break;
        case 2:
            cout << "FastFlow version" << endl;
            break;
        default:
            cerr << "Run mode not supported" << endl;
            return -1;
    }
    u.stop_timer();

    if(!found) {
        cout << "Node target not found" << endl;
    }

    return 0;
}
