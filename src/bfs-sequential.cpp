#include <iostream>
#include <random>
#include <deque>
#include <fstream>

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

    utimer executionTimer("Main thread");
    uint occurrences = 0;
    {
        vector<bool> visited(g.size(), false);

#if TIMER
        utimer bfsTimer("Main thread");
#endif
        deque<uint> frontier;
        visited[startingNode] = true;
        Node<uint> root = g[startingNode];
        frontier.push_back(startingNode);
#if TIMER
        bfsTimer.print("Preparing first frontier", bfsTimer.getElapsedTime());
        long nodeTime = 0;
        long nodeCounter = 0;
#endif
        while(!frontier.empty()) {
#if TIMER
            bfsTimer.reset();
#endif
            Node<uint> currentNode = g[frontier.front()];
            frontier.pop_front();
            occurrences += currentNode.first == target;
            for(size_t i = 0; i < currentNode.second.size(); i++) {
                uint pos = currentNode.second[i];
                if(!visited[pos])  {
                    visited[pos] = true;
                    frontier.push_back(pos);
                }
            }
#if TIMER
            nodeTime += bfsTimer.getElapsedTime();
            nodeCounter++;
#endif
        }
#if TIMER
        nodeTimer.print("Node time       ", nodeTime);
        nodeTimer.print("Node time (avg) ", nodeTime / nodeCounter);
#endif
    }
    executionTimer.print("BFS", executionTimer.getElapsedTime());
    
    std::cout << "Occurences found: " << occurrences << endl;
}
