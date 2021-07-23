/**
 * Performs a parallel breadth-first search using FastFlow.
 * 
 * It takes 5 positional arguments:
 *  - inputFile      : the path to the graph
 *  - startingNodeId : integer, the id of the from which the bfs will start
 *  - labelTarget    : integer, label whose occurrences are to be counted
 *  - nw             : integer, the number of workers to use
 *  - k              : integer, chunk size
 * 
 * @file    bfs-fastflow.cpp
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

#include <ff/ff.hpp>
#include <ff/farm.hpp>

using namespace ff;

/**
 * This method process a specific level of a bfs, it is expected
 * to be runs by multiple threads, hence it uses a dynamic auto 
 * scheduling.
 * 
 * @param g:            the input graph.
 * @param target:       the label to search.
 * @param tasks:        the data structured used to implements the 
 *                      scheduling mentioned.
 * @param nextFrontier: its a pointer to the next frontier, where
 *                      the nodes found will be added.
 * @param inserted:     its a vector of boolean used to decrease the
 *                      duplicates in the next frontier.
 * @param visited:      its the vector of visits, used to mark visited node
 *                      atomically.
 * @param mtx:          its a mutex used to write the local frontier to
 *                      the global one in mutally exclusive.
 * 
 * @return the number of occurrences found of the target label in the frontier.
 */
uint processLevel(
    Graph<uint> &g,
    uint target,
    TaskQueue &tasks,
    vector<uint> *nextFrontier,
    vector<bool> &inserted,
    vector<atomic_bool> &visited,
    mutex &mtx
) {
    /**
     * ***********************
     *     WORKING PHASE
     * ***********************
     */


    vector<uint> *frontier;
    frontier = tasks.get();
    uint localOccurrences = 0;
    vector<uint> localNextFrontier;
    pair<uint, uint> chunk;

    /**
     * This cycle ends when all the possible chunks were taken
     * by some of the threads, thus there is no more work available
     * for this level.
     */
    while(true) {

        /**
         * Retrieve a new task, <start,end> pair representing the
         * chunk of the frontier to visit.
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

            // Visit of the node.
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

    // Mutually exclusive write of the local next frontier.
    mtx.lock();
    nextFrontier->insert(nextFrontier->end(), localNextFrontier.begin(), localNextFrontier.end());
    mtx.unlock();

    localNextFrontier.clear();

    return localOccurrences;
};

/**
 * Master node structure typed with uint as input type,
 * since it receives at each iteration i the occurrences found
 * in the i-th level. The output type instead is a vector<uint>,
 * since to each thread the next frontier pointer is passed.
 */
struct Master: ff_monode_t<uint, vector<uint>> {
    /**
     * Master constructor.
     * 
     * @param nw:           the number of workers.
     * @param startingNode: the node where the bfs should start.
     * @param g:            the input graph.
     * @param target:       the label to search.
     * @param inserted:     its a vector of boolean used to decrease the
     *                      duplicates in the next frontier.
     * @param visited:      its the vector of visits, used to mark visited node
     *                      atomically.
     * @param mtx:          its a mutex used to write the local frontier to
     *                      the global one in mutally exclusive.
     * @param tasks:        the data structured used to implements the 
     *                      scheduling mentioned.
     */
    Master(
        uint nw,
        uint startingNode,
        Graph<uint> &g,
        uint target,
        vector<bool> &inserted,
        vector<atomic_bool> &visited,
        mutex &mtx,
        TaskQueue &tasks
    ): nw(nw), g(g), target(target), inserted(inserted), visited(visited), mtx(mtx), tasks(tasks) {
        workers = 0;
        occurrences = 0;
        frontier = new vector<uint>();
        nextFrontier = new vector<uint>();

        // Visits the root node.
        Node<uint> root = g[startingNode];
        inserted[startingNode] = true;
        visited[startingNode] = true;
        occurrences += root.first == target;

        // Adds root's neighborhood to the initial frontier.
        for(uint i = 0; i < root.second.size(); i++) {
            frontier->push_back(root.second[i]);
            inserted[root.second[i]] = true;
        }
        tasks.setFrontier(frontier);
    }

    /**
     * Master body.
     * 
     * @param workerOccurrences: its a pointer to a integer containing the
     * resulting number of occurrences found by a specific worker.
     * 
     * @return the next frontier pointer where the workers should write
     */
    vector<uint> *svc(uint *workerOccurrences) {
        /**
         * First call the frontier is send as is initialized with
         * the 0-level's neighbors.
         */
        if (workerOccurrences == nullptr) {
            /**
             * Send the next frontier pointer to the threads and
             * than start as them the level processing.
             */
            for(uint i = 0; i < nw - 1; i++) {
                ff_send_out(nextFrontier);
            }
            occurrences += processLevel(g, target, tasks, nextFrontier, inserted, visited, mtx);
        } else {
            workers++;
            occurrences += *workerOccurrences;

            // Level synchronization
            if(workers == nw - 1) {
                workers = 0;

                //If an empty next frontier is produced the visits ends.
                if(!nextFrontier->empty()) {

                    // Swap + clear
                    swap(frontier, nextFrontier);
                    nextFrontier->clear();
                    tasks.setFrontier(frontier);
                    
                    /**
                     * Send the new next frontier pointer to the threads and
                     * than start as them the level processing.
                     */
                    for(uint i = 0; i < nw - 1; i++) {
                        ff_send_out(nextFrontier);
                    }
                    occurrences += processLevel(g, target, tasks, nextFrontier, inserted, visited, mtx);
                } else {
                    return EOS;
                }
            }
        }
        return GO_ON;
    }

    uint nw;
    Graph<uint> &g;
    uint target;
    vector<bool> &inserted;
    vector<atomic_bool> &visited;
    uint occurrences;
    uint workers;
    mutex &mtx;
    vector<uint> *frontier;
    vector<uint> *nextFrontier;
    TaskQueue &tasks;
};

/**
 * Worker node structure typed with vector<uint> as input type,
 * since it receives at each iteration the next frontier pointer. 
 * The output type instead is a uint, since the occurrences found
 * in the i-th level will be returned through feedback channel.
 */
struct Worker: ff_node_t<vector<uint>, uint> {
    /**
     * Worker constructor.
     * 
     * @param g:            the input graph.
     * @param target:       the label to search.
     * @param inserted:     its a vector of boolean used to decrease the
     *                      duplicates in the next frontier.
     * @param visited:      its the vector of visits, used to mark visited node
     *                      atomically.
     * @param mtx:          its a mutex used to write the local frontier to
     *                      the global one in mutally exclusive.
     * @param tasks:        the data structured used to implements the 
     *                      scheduling mentioned.
     */
    Worker(
        Graph<uint> &g,
        uint target,
        vector<bool> &inserted,
        vector<atomic_bool> &visited,
        mutex &mtx,
        TaskQueue &tasks
    ): g(g), target(target), inserted(inserted), visited(visited), mtx(mtx), tasks(tasks) {
        occurrences = 0;
    }

    /**
     * Worker body, it process the level and returns the number of occurrences found.
     * 
     * @param nextFrontier: its a pointer to a vector of integer representing the
     *                      next frontier.
     * 
     * @result the number of occurrences found processing the level.
     */
    uint *svc(vector<uint> *nextFrontier) {
        occurrences = processLevel(g, target, tasks, nextFrontier, inserted, visited, mtx);
        ff_send_out(&occurrences);
        return GO_ON;
    }

    uint occurrences;
    Graph<uint> &g;
    uint target;
    vector<bool> &inserted;
    vector<atomic_bool> &visited;
    mutex &mtx;
    TaskQueue &tasks;
};

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
         * **********************
         * 
         * The master thread executes the initialization phase,
         * that can be seen as composed by 3 parts:
         *      1. The initialization of the data structure needed by the computation
         *      2. Setup the master-worker initializing both
         */

        // 1.1 The vector of visits
        vector<bool> inserted(g.size(), false);
        vector<atomic_bool> visited(g.size());
        for(uint i = 0; i < g.size(); i++) {
            visited[i] = false;
        }

        // 1.2 The mutex to write in mutually exclusive in the next frontier
        mutex mtx;

        // 1.3 The task queue parametric in k, the chunk size
        TaskQueue tasks(k);

        // 2.1 Initializes the farms with the node workers
        ff_Farm<> farm( [&]() {
            vector<unique_ptr<ff_node>> W;
            for(size_t i = 0; i < nw - 1;++i) {
                W.push_back(make_unique<Worker>(ref(g), target, ref(inserted), ref(visited), ref(mtx), ref(tasks)));
            }
            return W;
        } () );

        // 2.2 Initializes the master node
        Master master(nw, startingNode, ref(g), target, ref(inserted), ref(visited), ref(mtx), ref(tasks));
        farm.add_emitter(master);
        farm.remove_collector();
        farm.wrap_around();

        if (farm.run_and_wait_end()<0) {
            error("running farm\n");
            return -1;
        }

        occurrences = master.occurrences;
    }

    executionTimer.print("BFS", executionTimer.getElapsedTime());
    cout << "Occurences found: " << occurrences << endl;

    return 0;
}
