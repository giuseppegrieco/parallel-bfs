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
 * NOTE: this file provides different utilities for testing:
 *  - compiling with -DTEST repeat the bfs for each nw in the range [2, 32]
 * 
 * @file    bfs-fastflow-test.cpp
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

uint processLevel(
    Graph<uint> &g,
    uint target,
    TaskQueue &tasks,
    vector<uint> *nextFrontier,
    vector<bool> &inserted,
    vector<atomic_bool> &visited,
    mutex &mtx
) {
    vector<uint> *frontier;
    frontier = tasks.get();
    uint localOccurrences = 0;
    vector<uint> localNextFrontier;
    pair<uint, uint> chunk;
    while(true) {
        chunk = tasks.pop();
        if(chunk.second == 0) break;
        for(uint j = chunk.first; j < chunk.second; j++) {
            uint v = frontier->at(j);
            Node<uint> currentNode = g[v];
            bool boolFalse = false;
            if(!visited[v].compare_exchange_strong(
                boolFalse,
                true
            )) {
                continue;
            }
            localOccurrences += currentNode.first == target;
            for(size_t i = 0; i < currentNode.second.size(); i++) {
                uint pos = currentNode.second[i];
                if(inserted[pos]) {
                    continue;
                }
                localNextFrontier.push_back(pos);
                inserted[pos] = true;
            }
        }
    }
    mtx.lock();
    nextFrontier->insert(nextFrontier->end(), localNextFrontier.begin(), localNextFrontier.end());
    mtx.unlock();
    localNextFrontier.clear();
    return localOccurrences;
};


struct Master: ff_monode_t<uint, vector<uint>> {
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

        Node<uint> root = g[startingNode];
        inserted[startingNode] = true;
        visited[startingNode] = true;
        occurrences += root.first == target;
        for(uint i = 0; i < root.second.size(); i++) {
            frontier->push_back(root.second[i]);
            inserted[root.second[i]] = true;
        }
        tasks.setFrontier(frontier);
    }

    vector<uint> *svc(uint *workerOccurrences) {
        if (workerOccurrences == nullptr) {
            for(uint i = 0; i < nw - 1; i++) {
                ff_send_out(nextFrontier);
            }
            occurrences += processLevel(g, target, tasks, nextFrontier, inserted, visited, mtx);
        } else {
            workers++;
            occurrences += *workerOccurrences;
            if(workers == nw - 1) {
                workers = 0;
                if(!nextFrontier->empty()) {
                    swap(frontier, nextFrontier);
                    nextFrontier->clear();
                    tasks.setFrontier(frontier);
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

struct Worker: ff_node_t<vector<uint>, uint> {
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

    std::cout << "Graph read" << endl;
#if TEST
    for(uint nw = 2; nw <= 32; nw++) {
        acout() << "#nw" << nw << endl;
#endif
    atomic_uint occurrences;
    occurrences = 0;
    utimer executionTimer("Main thread");
    {
        vector<bool> inserted(g.size(), false);
        vector<atomic_bool> visited(g.size());
        for(uint i = 0; i < g.size(); i++) {
            visited[i] = false;
        }
        mutex mtx;
        TaskQueue tasks(k);

        ff_Farm<> farm( [&]() {
            std::vector<std::unique_ptr<ff_node>> W;
            for(size_t i = 0; i < nw - 1;++i) {
                W.push_back(make_unique<Worker>(ref(g), target, ref(inserted), ref(visited), ref(mtx), ref(tasks)));
            }
            return W;
        } () );

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
    std::cout << "Occurences found: " << occurrences << endl;
#if TEST
    }
#endif
    return 0;
}
