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

#if VISITED_TIMER
    atomic_long visitedTime(0);
    atomic_ulong visitedChecks(0);
#endif
#if NODE_TIMER
    atomic_long nodeTime(0);
    atomic_ulong nodeCounter(0);
#endif
#if W_TIMER
    atomic_ulong processTime(0);
    atomic_uint chunksProcessed(0);

    atomic_ulong popTime(0);
    atomic_ulong waitingTimer(0);

    atomic_uint levelsCounter(0); 
#endif
#if TEST
    for(uint nw = 2; nw <= 32; nw++) {
        acout() << "#nw" << nw << endl;
#endif
    atomic_uint occurrences;
    occurrences = 0;
    utimer executionTimer("Main thread");
    {
#if TIMER
        utimer preparationTimer("Main thread");
#endif
        vector<bool> inserted(g.size(), false);
        vector<atomic_bool> visited(g.size());
        for(uint i = 0; i < g.size(); i++) {
            visited[i] = false;
        }

        TaskQueue tasks(k);
        LevelSynchronization sync(nw);

        vector<uint> *nextFrontier = new vector<uint>();
        mutex mtx;
#if TIMER
        preparationTimer.print("1st stage visited array and utilities", preparationTimer.getElapsedTime());
#endif
        auto worker = [&](uint id, bool master) {
            vector<uint> *frontier;
            uint localOccurrences = 0;
            vector<uint> localNextFrontier;
            while(!(frontier = tasks.get())->empty()) {
                pair<uint, uint> chunk;
#if VISITED_TIMER
                utimer visitedTimer("Worker " + to_string(id));
#endif
#if W_TIMER
                utimer chunkTimer("Worker " + to_string(id));
#endif
#if NODE_TIMER
                utimer nodeTimer("Worker " + to_string(id));
#endif
                while(true) {
#if W_TIMER
                    chunkTimer.restart();
#endif
                    chunk = tasks.pop();
                    if(chunk.second == 0) break;
#if W_TIMER
                    popTime += chunkTimer.getElapsedTime();
                    chunkTimer.restart();
#endif
                    for(uint j = chunk.first; j < chunk.second; j++) {
#if NODE_TIMER
                        nodeTimer.restart();
#endif
                        uint v = frontier->at(j);
                        Node<uint> currentNode = g[v];
                        bool boolFalse = false;
#if VISITED_TIMER
                        visitedTimer.restart();
#endif
                        if(!visited[v].compare_exchange_strong(
                            boolFalse,
                            true
                        )) {
                            continue;
                        }
#if VISITED_TIMER
                        visitedTime += visitedTimer.getElapsedTime();
                        visitedChecks++;
#endif
                        localOccurrences += currentNode.first == target;
                        for(size_t i = 0; i < currentNode.second.size(); i++) {
                            uint pos = currentNode.second[i];
                            if(inserted[pos]) {
                                continue;
                            }
                            localNextFrontier.push_back(pos);
                            inserted[pos] = true;
                        }
#if NODE_TIMER
                        nodeTime += nodeTimer.getElapsedTime();
                        nodeCounter++;
#endif
                    }
#if W_TIMER
                    processTime += chunkTimer.getElapsedTime();
                    chunksProcessed++;
#endif
                }
#if W_TIMER
                chunkTimer.restart();
#endif
                mtx.lock();
#if W_TIMER
                chunkTimer.print("Writing time  (lock) ", chunkTimer.getElapsedTime());
                chunkTimer.restart();
#endif
                nextFrontier->insert(nextFrontier->end(), localNextFrontier.begin(), localNextFrontier.end());
                mtx.unlock();
                localNextFrontier.clear();
#if W_TIMER
                chunkTimer.print("Writing time   ", chunkTimer.getElapsedTime());
#endif
                if(master) {
#if W_TIMER
                    chunkTimer.restart();
#endif
                    sync.waitSlaves();
#if W_TIMER
                    waitingTimer += chunkTimer.getElapsedTime();
#endif
                    swap(frontier, nextFrontier);
                    nextFrontier->clear();
                    tasks.setFrontier(frontier);
                    sync.reset();
                } else {
#if W_TIMER
                    chunkTimer.restart();
#endif
                    sync.increment();
                    sync.waitMaster();
#if W_TIMER
                    waitingTimer += chunkTimer.getElapsedTime();
#endif
                }
#if W_TIMER
                levelsCounter++;
#endif
            }
            occurrences += localOccurrences;
        };

#if TIMER
        preparationTimer.restart();
#endif
        vector<uint> *frontier = new vector<uint>();
        Node<uint> root = g[startingNode];
        inserted[startingNode] = true;
        visited[startingNode] = true;
        occurrences += root.first == target;
        for(uint i = 0; i < root.second.size(); i++) {
            frontier->push_back(root.second[i]);
            inserted[root.second[i]] = true;
        }
        tasks.setFrontier(frontier);
#if TIMER
        preparationTimer.print("2st stage, frontier initialization", preparationTimer.getElapsedTime());
        preparationTimer.restart();
#endif
        vector<thread> pool(nw - 1);
        for (uint i = 0; i < nw - 1; i++) {
            pool[i] = std::thread(
                worker, 
                i,
                false
            );
        }
#if TIMER
        preparationTimer.print("3st stage, Thread creation ", preparationTimer.getElapsedTime());
#endif
        worker(nw, true);

#if TIMER
        preparationTimer.restart();
#endif
        for (uint i = 0; i < nw - 1; i++) {
            pool[i].join();
        }
#if TIMER
        preparationTimer.print("3st stage, Thread join", preparationTimer.getElapsedTime());
#endif
    }
    executionTimer.print("BFS", executionTimer.getElapsedTime());
    std::cout << "Occurences found: " << occurrences << endl;
#if W_TIMER
    executionTimer.print("Waiting time   ", waitingTimer);
    executionTimer.print("Waiting time (avg) ", waitingTimer / levelsCounter);
    executionTimer.print("Chunks (total) ", processTime);
    executionTimer.print("Chunks (avg)   ", processTime / chunksProcessed);
    executionTimer.print("Pop (total)    ", popTime);
    executionTimer.print("Pop (avg)      ", popTime / chunksProcessed);
#endif
#if NODE_TIMER
    executionTimer.print("Node time       ", nodeTime);
    executionTimer.print("Node time (avg) ", nodeTime / nodeCounter);
#endif
#if VISITED_TIMER
    executionTimer.print("Visited checks       ", visitedTime);
    executionTimer.print("Visited checks (avg) ", visitedTime / visitedChecks);
#endif
#if TEST
    }
#endif
    return 0;
}