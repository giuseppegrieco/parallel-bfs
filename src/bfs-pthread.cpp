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

bool todo = true;

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
    {
        utimer u("bfs");
        
        vector<atomic_bool> visited(g.size());
        for(size_t i = 0; i < g.size(); i++) {
            visited[i] = false;
        }

        vector<thread> pool(nw - 1);
        LevelSynchronization sync(nw);
        TaskQueue *inputQueue(new TaskQueue());
        TaskQueue *outputQueue(new TaskQueue());

        task init;
        visited[startingNode] = true;
        init.frontier.push(startingNode);
        inputQueue->add(init);

        auto worker = [&](uint id, bool master) {
            uint localOccurrences = 0;
#if VERBOSE
            uint lv = 0;
            acout() << "Thread " << id << " : " << " started " << endl;
#endif
            while(todo) {
        /**
         * Process current-level tasks
         */
#if VERBOSE
                acout() << "Thread " << id << " : " << "Level " << lv << " started" << endl;
#endif
                task levelTask;
                while(!(levelTask = inputQueue->pop()).none) {
                    task nextLevelTask;
                    /** 
                     * Process task frontier 
                     */
                    while(!levelTask.frontier.empty()) {
                        Node<uint> currentNode = g[levelTask.frontier.front()];
                        levelTask.frontier.pop();
                        localOccurrences += currentNode.first == target;

                        for(size_t i = 0; i < currentNode.second.size(); i++) {
                            uint pos = currentNode.second[i];
                            bool booleanFalse = false;
                            if(visited[pos].compare_exchange_strong(
                                booleanFalse,
                                true
                            )) {
                                if(nextLevelTask.frontier.size() == k) {
                                    outputQueue->add(nextLevelTask);
                                    task empty;
                                    nextLevelTask = empty;
                                }
                                nextLevelTask.frontier.push(pos);
                            }
                        }
                    }
                    if(!nextLevelTask.frontier.empty()) {
                        outputQueue->add(nextLevelTask);
                    }
                }
#if VERBOSE
                acout() << "Thread " << id << " : " << "Level " << lv << " terminated" << endl;
#endif
                /**
                 * Level-synchronization
                 */
#if VERBOSE
                acout() << "Thread " << id << " : " << "sync..." << endl;
#endif
                if(master) {
                    sync.waitSlaves();
                    todo = !outputQueue->empty();
                    swap(inputQueue, outputQueue);
                    sync.reset();
                } else {
                    sync.increment();
                    sync.waitMaster();
                }
#if VERBOSE
                acout() << "Thread " << id << " : " << "synchronized" << endl;
                lv++;
#endif
            }
            /**
             * Sum the occurrences found
             */
            occurrences += localOccurrences;
#if VERBOSE
            acout() << "Thread " << id << " : " << " Terminated, occurrences found " << localOccurrences << endl;
#endif
        };

#if VERBOSE
            std::cout << "Workers are starting..." << endl;
#endif
        for (uint i = 0; i < nw - 1; i++) {
            pool[i] = std::thread(worker, i, false);
        }
        worker(nw, true);

        for (uint i = 0; i < nw - 1; i++) {
            pool[i].join();
        }
    }
    
    std::cout << "Occurences found: " << occurrences << endl;
}