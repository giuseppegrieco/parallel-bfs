/**
 * Performs a parallel breadth-first search using pthread.
 * 
 * It takes 5 positional arguments:
 *  - inputFile      : the path to the graph
 *  - startingNodeId : integer, the id of the from which the bfs will start
 *  - labelTarget    : integer, label whose occurrences are to be counted
 *  - nw             : integer, the number of workers to use
 *  - k              : integer, chunk size
 * 
 * @file    bfs-pthread.cpp
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
    if(argc < 6) {
        cerr << "Usage: " << argv[0] << " inputFile startingNodeId labelTarget nw k\n";
        cerr << "positional arguments:\n";
        cerr << "\tinputFile      : string, the path to the graph\n";
        cerr << "\tstartingNodeId : integer, the id of the from which the bfs will start\n";
        cerr << "\tlabelTarget    : integer, label whose occurrences are to be counted\n";
        cerr << "\tnw             : integer, the number of workers to use\n";
        cerr << "\tk              : integer, chunk size\n";
        return -1;
    }
    string inputFile = argv[1];
    uint startingNode = stoi(argv[2]);
    uint target = stoi(argv[3]);
    uint nw = stoi(argv[4]);
    uint k = stoi(argv[5]);

    cout << "Reading the run from the file." << endl;
    Graph<uint> g = read<uint>(inputFile);
    cout << "Graph read" << endl;

    atomic_uint occurrences;
    occurrences = 0;
    utimer executionTimer("Main thread");
    {
        /**
         * ***********************
         *  INITIALIZATION PHASE
         * ***********************
         * 
         * The master thread executes the initialization phase,
         * that can be seen as composed by 3 parts:
         *      1. The initialization of the data structure needed by the computation
         *      2. The initialization of the frontier, i.e. visits the level 0
         *      3. The creation of the worker threads
         */

        // 1.1 The vector of visits
        vector<bool> inserted(g.size(), false);
        vector<atomic_bool> visited(g.size());
        for(uint i = 0; i < g.size(); i++) {
            visited[i] = false;
        }

        // 1.2 The task queue parametric in k, the chunk size
        TaskQueue tasks(k);

        // 1.3 The object that implements the level synchronization
        LevelSynchronization sync(nw);

        // 1.4 The pointer to the next frontier that the workers share
        vector<uint> *nextFrontier = new vector<uint>();

        // 1.5 The mutex to write in mutually exclusive in the next frontier
        mutex mtx;

        auto worker = [&](uint id, bool master) {
            // A pointer to the current frontier
            vector<uint> *frontier = tasks.get();
            uint localOccurrences = 0;
            vector<uint> localNextFrontier;

            /**
             * This cycle ends when the visited is completed,
             * i.e. when an empty frontier is produced
             */
            do {
                pair<uint, uint> chunk;

                /**
                 * This cycle ends when all the possible chunks were taken
                 * by some of the threads, thus there is no more work available
                 * for this level.
                 */
                while(true) {

                    /**
                     * Retrieve a new task, <start,end> pair representing the
                     * chunk of the frontier to visit
                    */
                    chunk = tasks.pop();
                    if(chunk.second == 0) break;

                    /**
                     * This for-cycle visits all the index in the range indicated 
                     * by the task.
                    */
                    for(uint j = chunk.first; j < chunk.second; j++) {
                        uint v = frontier->at(j);
                        Node<uint> currentNode = g[v];
                        bool boolFalse = false;

                        // Atomically check if the node has already been visited.
                        if(!visited[v].compare_exchange_strong(
                            boolFalse,
                            true
                        )) {
                            continue;
                        }

                        // Visit of the node
                        localOccurrences += currentNode.first == target;

                        /**
                         * This cycle go through each node in the neighborhood 
                         * of the visiting node.
                         */
                        for(size_t i = 0; i < currentNode.second.size(); i++) {
                            uint pos = currentNode.second[i];

                            /**
                             * This non-atomic check try to reduce the number of
                             * possible duplicates in the frontier helping to decrease
                             * the time in which the atomic checks fails, exploting
                             * pipeline through branch prediction.
                             */
                            if(inserted[pos]) {
                                continue;
                            }

                            localNextFrontier.push_back(pos);
                            inserted[pos] = true;
                        }
                    }
                }

                // Mutually exclusive write of the local next frontier
                mtx.lock();
                nextFrontier->insert(nextFrontier->end(), localNextFrontier.begin(), localNextFrontier.end());
                mtx.unlock();
                localNextFrontier.clear();
                
                // Level synchronization
                if(master) {
                    sync.waitWorkers();

                    // Swap + merge
                    swap(frontier, nextFrontier);
                    nextFrontier->clear();
                    tasks.setFrontier(frontier);

                    sync.reset();
                } else {
                    sync.increment();
                }
                sync.wait();

            } while(!(frontier = tasks.get())->empty());
            occurrences += localOccurrences;
        };
        vector<uint> *frontier = new vector<uint>();

        // 1.1 Visits the root node.
        Node<uint> root = g[startingNode];
        inserted[startingNode] = true;
        visited[startingNode] = true;
        occurrences += root.first == target;

        // 1.2 Adds root's neighborhood to the initial frontier.
        for(uint i = 0; i < root.second.size(); i++) {
            frontier->push_back(root.second[i]);
            inserted[root.second[i]] = true;
        }
        tasks.setFrontier(frontier);
        
        // 3. It make usage of a thread pool to starts and efficiently reuse the workers
        vector<thread> pool(nw - 1);
        for (uint i = 0; i < nw - 1; i++) {
            pool[i] = thread(
                worker, 
                i,
                false
            );
        }

        /**
         * ***********************
         *     WORKING PHASE
         * ***********************
         */
        worker(nw, true);

        // Thread joining
        for (uint i = 0; i < nw - 1; i++) {
            pool[i].join();
        }
    }

    executionTimer.print("BFS", executionTimer.getElapsedTime());
    cout << "Occurences found: " << occurrences << endl;

    return 0;
}
