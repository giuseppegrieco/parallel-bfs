#include <iostream>
#include <atomic>
#include <thread>
#include <queue>

#include "graph.hpp"
#include "utimer.hpp"
#include "sync.hpp"

#if VERBOSE
#include "acout.hpp"
#endif

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 6) {
        std::cerr << "Usage: " << argv[0] << " inputFile startingNodeId labelTarget nw k\n";
        std::cerr << "positional arguments:\n";
        std::cerr << "\tinputFile      : string, the path to the graph\n";
        std::cerr << "\tstartingNodeId : integer, the id of the from which the bfs will start\n";
        std::cerr << "\tlabelTarget    : integer, label whose occurrences are to be counted\n";
        std::cerr << "\tnw             : integer, the number of workers to use\n";
        std::cerr << "\tk              : integer, chunk size\n";
        return -1;
    }
    string inputFile = argv[1];
    uint startingNode = stoi(argv[2]);
    uint target = stoi(argv[3]);
    uint nw = stoi(argv[4]);
    uint k = stoi(argv[5]);

    std::cout << "Reading the run from the file." << endl;

    Graph<uint> g = read<uint>(inputFile);

    std::cout << "Graph readed" << endl;

    atomic_uint occurrences;
    occurrences = 0;
    
    std::cout << "Occurences found: " << occurrences << endl;
    return 0;
}