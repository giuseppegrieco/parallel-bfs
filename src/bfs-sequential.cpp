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
        return -1;
    }
    string inputFile = argv[1];
    uint startingNode = stoi(argv[2]);
    uint target = stoi(argv[3]);

    std::cout << "Reading the run from the file." << endl;

    Graph<uint> g = read<uint>(inputFile);

    std::cout << "Graph readed" << endl;

    uint occurrences;
    occurrences = 0;

#if TIMER
        utimer bfsTimer("Main thread");
        utimer nodeTimer("Main thread");
        long nodeTime = 0;
        long nodeCounter = 0;
#endif
#if I_TIMER
        utimer istrTimer("Main thread");
		long posTime = 0;
		long visitedCheckTime = 0;
		long visitedSetTime = 0;
		long pushTime = 0;
		long occurrencesTime = 0;
		long getNodeTime = 0;
        long accessFrontier = 0;
		long swapTime = 0;
		long clearTime = 0;
		ulong lvs = 0;
		ulong outCounter = 0;
		ulong inCounter = 0;
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
#if I_TIMER
				istrTimer.restart();
#endif
                uint v = frontier->at(j);
#if I_TIMER
				accessFrontier += istrTimer.getElapsedTime();
				istrTimer.restart();
#endif
                Node<uint> currentNode = g[v];
#if I_TIMER
				getNodeTime += istrTimer.getElapsedTime();
				istrTimer.restart();
#endif
                occurrences += currentNode.first == target;
#if I_TIMER
				occurrencesTime += istrTimer.getElapsedTime();
#endif
#if TIMER
                nodeTimer.restart();
#endif
                for(size_t i = 0; i < currentNode.second.size(); i++) {
#if I_TIMER
					istrTimer.restart();
#endif
                    uint pos = currentNode.second[i];
#if I_TIMER
					posTime += istrTimer.getElapsedTime();
					istrTimer.restart();
#endif
                    if(visited[pos]) {
                        continue;
                    }
#if I_TIMER
					visitedCheckTime += istrTimer.getElapsedTime();
					istrTimer.restart();
#endif
                    nextFrontier->push_back(pos);
#if I_TIMER
					pushTime += istrTimer.getElapsedTime();
					istrTimer.restart();
#endif
                    visited[pos] = true;
#if I_TIMER
					visitedSetTime += istrTimer.getElapsedTime();
					inCounter++;
#endif
                }
#if TIMER
                if(currentNode.second.size() > 0) {
                    nodeTime += nodeTimer.getElapsedTime() / currentNode.second.size();
                    nodeCounter++;
                }
#endif
#if I_TIMER
				outCounter++;
#endif
            }
#if I_TIMER
			istrTimer.restart();
#endif
            swap(frontier, nextFrontier);
#if I_TIMER
			swapTime += istrTimer.getElapsedTime();
			istrTimer.restart();
#endif
            nextFrontier->clear();
#if I_TIMER
            clearTime += istrTimer.getElapsedTime();
			lvs++;
#endif
        }
    }
    executionTimer.print("BFS", executionTimer.getElapsedTime());
#if I_TIMER
        istrTimer.print("Clear time               ", clearTime);
        istrTimer.print("Clear time (avg)         ", clearTime / lvs);
        istrTimer.print("Swap time                ", swapTime);
        istrTimer.print("Swap time (avg)          ", swapTime / lvs);
        istrTimer.print("Access frontier time     ", accessFrontier);
        istrTimer.print("Access frontier (avg)    ", accessFrontier / outCounter);
        istrTimer.print("Get current node time    ", getNodeTime);
        istrTimer.print("Get current node (avg)   ", getNodeTime / outCounter);
        istrTimer.print("Count occurrences time   ", occurrencesTime);
        istrTimer.print("Count occurrences (avg)  ", occurrencesTime / outCounter);
        istrTimer.print("Get children pos time    ", posTime);
        istrTimer.print("Get children pos (avg)   ", posTime / inCounter);
        istrTimer.print("Check visited node time  ", visitedCheckTime);
        istrTimer.print("Check visited node (avg) ", visitedCheckTime / inCounter);
        istrTimer.print("Set visited node time    ", visitedSetTime);
        istrTimer.print("Set visited node (avg)   ", visitedSetTime / inCounter);
        istrTimer.print("Push child time          ", pushTime);
        istrTimer.print("Push child (avg)         ", pushTime / inCounter);
#endif
#if TIMER
        nodeTimer.print("Node time       ", nodeTime);
        nodeTimer.print("Node time (avg) ", nodeTime / nodeCounter);
#endif
    std::cout << "Occurences found: " << occurrences << endl;
    return 0;
}