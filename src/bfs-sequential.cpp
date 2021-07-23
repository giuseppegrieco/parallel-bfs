/**
 * Performs a sequential breadth-first search.
 * 
 * It takes 3 positional arguments:
 *  - inputFile      : the path to the graph
 *  - startingNodeId : integer, the id of the from which the bfs will start
 *  - labelTarget    : integer, label whose occurrences are to be counted
 * 
 * @file    bfs-sequential.cpp
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

int main(int argc, char *argv[]) {
    if(argc < 4) {
        cerr << "Usage: " << argv[0] << " inputFile startingNodeId labelTarget nw k\n";
        cerr << "positional arguments:\n";
        cerr << "\tinputFile      : string, the path to the graph\n";
        cerr << "\tstartingNodeId : integer, the id of the from which the bfs will start\n";
        cerr << "\tlabelTarget    : integer, label whose occurrences are to be counted\n";
        return -1;
    }
    string inputFile = argv[1];
    uint startingNode = stoi(argv[2]);
    uint target = stoi(argv[3]);

    cout << "Reading the run from the file." << endl;
    Graph<uint> g = read<uint>(inputFile);
    cout << "Graph read" << endl;
    
    uint occurrences = 0;
    utimer executionTimer("Main thread");
    {
        /**
         * ***********************
         *  INITIALIZATION PHASE
         * ***********************
         */

        // Initialize the vector of visits
        vector<bool> visited(g.size(), false);

        // Initialize the frontier
        vector<uint> *frontier = new vector<uint>();

        // Initialize the next frontier
        vector<uint> *nextFrontier = new vector<uint>();

        // Visits the root node.
        Node<uint> root = g[startingNode];
        visited[startingNode] = true;
        occurrences += root.first == target;

        // Adds root's neighborhood to the initial frontier.
        for(uint i = 0; i < root.second.size(); i++) {
            frontier->push_back(root.second[i]);
            visited[root.second[i]] = true;
        }

        /**
         * ***********************
         *     WORKING PHASE
         * ***********************
         */

        /**
         * This cycle ends when the visited is completed,
         * i.e. when an empty frontier is produced
         */
        while(!frontier->empty()) {
            /**
             * This cycle iterates over all the frontier visiting
             * each node.
             */
            for(uint j = 0; j < frontier->size(); j++) {

                // Visit of the node
                uint v = frontier->at(j);
                Node<uint> currentNode = g[v];
                occurrences += currentNode.first == target;

                /**
                 * This cycle go through each node in the neighborhood 
                 * of the visiting node.
                 */
                for(size_t i = 0; i < currentNode.second.size(); i++) {
                    uint pos = currentNode.second[i];
                    if(visited[pos]) { // avoid duplicates
                        continue;
                    }
                    nextFrontier->push_back(pos);
                    visited[pos] = true;
                }
            }

            // Swap + clear
            swap(frontier, nextFrontier);
            nextFrontier->clear();
        }
    }

    executionTimer.print("BFS", executionTimer.getElapsedTime());
    cout << "Occurences found: " << occurrences << endl;
    
    return 0;
}