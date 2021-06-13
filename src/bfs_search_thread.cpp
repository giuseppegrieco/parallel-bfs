#include "bfs_search.h"

bool bfs_search_thread(int target, vector<node> nodes, int nw) {
    queue<int> frontier;
    nodes[0].visited = true;
    frontier.push(0);
    bool found = false;
    
    while(!frontier.empty() && !found) {
        node currentNode = nodes[frontier.front()];
        frontier.pop();
        cout << "Visited node: " << currentNode.label << endl;

        if(currentNode.label == target) {
            cout << "Node target found" << endl;
            found = true;
        }
        
        for(size_t i = 0; i < currentNode.adj.size(); i++) {
            int pos = currentNode.adj[i];
            node currentChild = nodes[pos];
            if(currentChild.visited == false) {
                currentChild.visited = true;
                frontier.push(pos);
            }
        }
    }
    return found;
}
