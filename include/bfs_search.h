#include <vector>
#include <queue>
#include <iostream>
#include <thread>
#include <deque>

#include "graph_utils.h"
#include "syque.h"

using namespace std;

int bfs_search_seq(int target, int startingNode, vector<node> nodes);
int bfs_search_thread(int target, int startingNode, vector<node> nodes, int nw);
