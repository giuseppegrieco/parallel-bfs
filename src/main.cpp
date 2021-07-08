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

    cout << "Reading the run from the file." << endl;
    tuple<int, int, vector<node>> run = load_run_from_file(inputFile);
    cout << "Run loaded" << endl;

    int target = get<0>(run);
    int startingNode = get<1>(run);
    cout << "Node target: " << target << endl;
    vector<node> nodes = get<2>(run);

    int occurrences = 0;
    {
        utimer u("bfs");
        switch(runMode) {
            case 0:
                cout << "Sequential version" << endl;
                occurrences = bfs_search_seq(target, startingNode, nodes);
                break;
            case 1:
                cout << "Thread version" << endl;
                occurrences = bfs_search_thread(target, startingNode, nodes, nw);
                break;
            case 2:
                cout << "FastFlow version" << endl;
                break;
            default:
                cerr << "Run mode not supported" << endl;
                return -1;
        }
    }
    cout << "Occurrences of target label: " << occurrences << endl;
    return 0;
}
