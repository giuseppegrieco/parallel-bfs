#include <vector>
#include <queue>
#include <iostream>
#include <thread>

#include "graph_utils.h"

using namespace std;

bool bfs_search_seq(int target, vector<node> nodes);
bool bfs_search_thread(int target, vector<node> nodes, int nw);
