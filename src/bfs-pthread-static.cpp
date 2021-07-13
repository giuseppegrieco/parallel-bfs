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
    if(argc < 5) {
        std::cerr << "Usage: " << argv[0] << " inputFile startingNodeId labelTarget nw k\n";
        std::cerr << "positional arguments:\n";
        std::cerr << "\tinputFile      : string, the path to the graph\n";
        std::cerr << "\tstartingNodeId : integer, the id of the from which the bfs will start\n";
        std::cerr << "\tlabelTarget    : integer, label whose occurrences are to be counted\n";
        std::cerr << "\tnw             : integer, the number of workers to use\n";
        return -1;
    }
    string inputFile = argv[1];
    uint startingNode = stoi(argv[2]);
    uint target = stoi(argv[3]);
    uint nw = stoi(argv[4]);

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
        vector<int> *inputQueue(new vector<int>());
        TaskQueue *outputQueue(new TaskQueue());

        visited[startingNode] = true;
        inputQueue->push_back(startingNode);

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
                task task;
                size_t s =  id * (inputQueue->size() / nw);
                size_t e =  min((id+1) * (inputQueue->size() / nw) + 1, inputQueue->size());
#if VERBOSE
                acout() << inputQueue->size() << " " << s << " fuck " << e << endl;
#endif
                if(s < e) {
                    for(size_t i = s; i < e; i++) {
                        Node<uint> currentNode = g[inputQueue->at(i)];
                        localOccurrences += currentNode.first == target;
                        
                        for(size_t i = 0; i < currentNode.second.size(); i++) {
                            uint pos = currentNode.second[i];
                            bool booleanFalse = false;
                            if(visited[pos].compare_exchange_strong(
                                booleanFalse,
                                true
                            )) {
                                task.frontier.push_back(pos);
                            }
                        }
                    }
                }
                outputQueue->add(task);
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
                    inputQueue = new vector<int>();
                    for(size_t i = 0; i < nw; i++) {
                        task = outputQueue->pop();
                        inputQueue->insert(inputQueue->end(), task.frontier.begin(), task.frontier.end());
                    }
                    todo = !inputQueue->empty();
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
        worker(nw - 1, true);

        for (uint i = 0; i < nw - 1; i++) {
            pool[i].join();
        }
    }
    
    std::cout << "Occurences found: " << occurrences << endl;
}