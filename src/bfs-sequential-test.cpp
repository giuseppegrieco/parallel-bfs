/**
 * Performs a sequential breadth-first search.
 * 
 * It takes 3 positional arguments:
 *  - inputFile      : the path to the graph
 *  - startingNodeId : integer, the id of the from which the bfs will start
 *  - labelTarget    : integer, label whose occurrences are to be counted
 * 
 * NOTE: this file provides different utilities for testing:
 *  - compiling with -DTEST repeat the bfs 10 times
 *  - compiling with -DTIMER collects different times and print them at the end
 * 
 * @file    bfs-sequential-test.cpp
 * @author  Giuseppe Grieco
 * @version 1.0 22/06/21
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <queue>

#include "graph.hpp"
#include "utimer.hpp"
#include "sync.hpp"

using namespace std;

int main(int argc, char *argv[]) {
    if(argc < 4) {
        std::cerr << "Usage: " << argv[0] << " inputFile startingNodeId labelTarget nw k\n";
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

    std::cout << "Graph read" << endl;
#if TEST
    for(uint attempt = 0; attempt < 10; attempt++) {
#endif
    uint occurrences;
    occurrences = 0;
#if TIMER
        utimer bfsTimer("Main thread");

		long getNodeTime = 0;
        long accessFrontier = 0;

        long neighborTime = 0;

		long swapTime = 0;
		long clearTime = 0;

        long nodeCounter = 0;
        long nodeNCounter = 0;
        long lvs = 0;
#endif
    utimer executionTimer("Main thread");
    {
        vector<bool> visited(g.size(), false);
        vector<uint> *frontier = new vector<uint>();
        vector<uint> *nextFrontier = new vector<uint>();
        Node<uint> root = g[startingNode];
        visited[startingNode] = true;
        occurrences += root.first == target;
        for(uint i = 0; i < root.second.size(); i++) {
            frontier->push_back(root.second[i]);
            visited[root.second[i]] = true;
        }
#if TIMER
        bfsTimer.print("Preparing first frontier", bfsTimer.getElapsedTime());
#endif
        while(!frontier->empty()) {
            for(uint j = 0; j < frontier->size(); j++) {
#if TIMER
				bfsTimer.restart();
#endif
                uint v = frontier->at(j);
#if TIMER
				accessFrontier += bfsTimer.getElapsedTime();
				bfsTimer.restart();
#endif
                Node<uint> currentNode = g[v];
#if TIMER
				getNodeTime += bfsTimer.getElapsedTime();
#endif
                occurrences += currentNode.first == target;
#if TIMER
                bfsTimer.restart();
#endif
                for(size_t i = 0; i < currentNode.second.size(); i++) {
                    uint pos = currentNode.second[i];
                    if(visited[pos]) {
                        continue;
                    }
                    nextFrontier->push_back(pos);
                    visited[pos] = true;
                }
#if TIMER
                ulong neighborhoodTime = bfsTimer.getElapsedTime();
                if(currentNode.second.size() > 0) {
                    neighborTime += neighborhoodTime  / currentNode.second.size();
                    nodeNCounter++;
                }
                nodeCounter++;
#endif
            }
#if TIMER
			bfsTimer.restart();
#endif
            swap(frontier, nextFrontier);
#if TIMER
			swapTime += bfsTimer.getElapsedTime();
			bfsTimer.restart();
#endif
            nextFrontier->clear();
#if TIMER
            clearTime += bfsTimer.getElapsedTime();
			lvs++;
#endif
        }
    }
    executionTimer.print("BFS", executionTimer.getElapsedTime());
#if TIMER
    bfsTimer.print("Access frontier time     ", accessFrontier);
    bfsTimer.print("Access frontier (avg)    ", accessFrontier / nodeCounter);

    bfsTimer.print("Get current node time    ", getNodeTime);
    bfsTimer.print("Get current node (avg)   ", getNodeTime / nodeCounter);

    bfsTimer.print("Neighbor time       ", neighborTime);
    bfsTimer.print("Neighbor time (avg) ", neighborTime / nodeNCounter);

    bfsTimer.print("Clear time               ", clearTime);
    bfsTimer.print("Clear time (avg)         ", clearTime / lvs);

    bfsTimer.print("Swap time                ", swapTime);
    bfsTimer.print("Swap time (avg)          ", swapTime / lvs);
#endif
    std::cout << "Occurences found: " << occurrences << endl;
#if TEST
    }
#endif
    return 0;
}
