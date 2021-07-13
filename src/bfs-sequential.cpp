#include <iostream>
#include <random>
#include <queue>

#include "graph.hpp"
#include "utimer.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cerr << "Usage: " << argv[0] << " inputFile startingNodeId labelTarget\n\n";

        std::cerr << "positional arguments:\n";
        std::cerr << "\tinputFile      : string, the path to the graph\n";
        std::cerr << "\tstartingNodeId : integer, the id of the from which the bfs will start\n";
        std::cerr << "\tlabelTarget    : integer, label whose occurrences are to be counted\n";
        return -1;
    }
    string inputFile = argv[1];
    uint startingNode = stoi(argv[2]);
    uint target = stoi(argv[3]);
    
    std::cout << "Reading the run from the file." << endl;
    Graph<uint> g = read<uint>(inputFile);
    std::cout << "Graph readed" << endl;

    uint occurrences = 0;
    {
        utimer u("bfs");

        vector<bool> visited(g.size(), false);

        queue<uint> frontier;
        visited[startingNode] = true;
        frontier.push(startingNode);

        while(!frontier.empty()) {
            Node<uint> currentNode = g[frontier.front()];
            frontier.pop();
            occurrences += currentNode.first == target;
            for(size_t i = 0; i < currentNode.second.size(); i++) {
                uint pos = currentNode.second[i];
                if(!visited[pos])  {
                    visited[pos] = true;
                    frontier.push(pos);
                }
            }
        }
    }
    
    std::cout << "Occurences found: " << occurrences << endl;
}
