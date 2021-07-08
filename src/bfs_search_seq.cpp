#include "bfs_search.h"

int bfs_search_seq(int target, int startingNode, vector<node> nodes) {
    queue<int> frontier;
    nodes[startingNode].visited = true;
    frontier.push(startingNode);
    int occurrences = 0;

    while(!frontier.empty()) {
        node currentNode = nodes[frontier.front()];
        frontier.pop();

        occurences += current.label == target;
        for(size_t i = 0; i < currentNode.adj.size(); i++) {
            int pos = currentNode.adj[i];
            if(nodes[pos].visited == false) {
                nodes[pos].visited = true;
                frontier.push(pos);
            }
        }
    }
    return occurrences;
}
