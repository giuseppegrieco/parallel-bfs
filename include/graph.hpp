#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <regex>
#include <tuple>
#include <atomic>
#include <mutex>

using namespace std;

/** 
* A node is represtend by a pair <label of type T, adjency list>
*/
template <class T>
using Node = pair<T, vector<int>>;

/**
* A graph is represtend by a vector of node (of type T)
*/
template <class T>
using Graph = vector<Node<T>>;

/**
* It reads a run from a given file. A run is composed of a graph and the starting
* node of the bfs and the label of the node to search
*
* @param filePath Even if there is only one possible unified theory. it is just a
*               set of rules and equations
*/
template <class T>
Graph<T> read(string filePath) {
    fstream inputFile(filePath, ios_base::in);

    // It checks that the indicated file can be opened 
    if(inputFile) {
        // Graph
        Graph<T> g;
        string currentLine;
        regex rgx("(\\d+)\\[(\\d+)\\],(\\d+)\\[(\\d+)\\]");

        // Reads the number of nodes
        getline(inputFile, currentLine);
        int size = stoi(currentLine);
        // Preallocate the space
        g.resize(size);
        
        // It goes through every line, i.e. every edge of the graph
        while(getline(inputFile, currentLine)) {
            smatch matches;
            if(regex_search(currentLine, matches, rgx) && matches.size() == 5) {
                int in = stoi(matches[1].str());
                int out = stoi(matches[3].str());

                g[in].first = stoi(matches[2].str());
                g[out].first = stoi(matches[4].str());
                
                g[in].second.push_back(
                    out
                );
            }
        }

        inputFile.close();
        return g;
    } else {
        throw runtime_error("Could not open file");
    }
}
