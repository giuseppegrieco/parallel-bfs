#include  "graph_utils.h"

tuple<int, int, vector<node>> load_run_from_file(string filePath) {
    regex rgx("(\\d+)\\[(\\d+)\\],(\\d+)\\[(\\d+)\\]");
    fstream inputFile(filePath, ios_base::in);
    node startingNode;

    // Create a map of node's label -> position of the node in vector
    map<int, int> nodePositions;

    // Graph nodes
    vector<node> nodes;

    // It checks that the indicated file can be opened 
    if(inputFile) {
        string currentLine;

        // Reads the label of the node to search
        getline(inputFile, currentLine);
        int target = stoi(currentLine);

        // Creates the starting node and put it on position 0
        getline(inputFile, currentLine);
        int startingId = stoi(currentLine);
        int startingNode = -1;
        
        // It goes through every line, i.e. every edge of the graph
        while(getline(inputFile, currentLine)) {
            smatch matches;
            if(regex_search(currentLine, matches, rgx) && matches.size() == 5) {
                int in = stoi(matches[1].str());
                int inLabel = stoi(matches[2].str());
                int out = stoi(matches[3].str());
                int outLabel = stoi(matches[4].str());

                // Checks if the node are already found previously, if not
                // add them to the nodes list
                create_node(in, inLabel, nodes, nodePositions);
                create_node(out, outLabel, nodes, nodePositions);
                
                if(startingNode == -1) {
                    if(startingId == in) {
                        startingNode = nodePositions[in];
                    } else if(startingId == out) {
                        startingNode = nodePositions[out];
                    }
                }

                nodes[nodePositions[in]].adj.push_back(
                    nodePositions[out]
                );
            }
        }

        inputFile.close();
        tuple<int, int, vector<node>> res;
        get<0>(res) = target;
        get<1>(res) = startingNode;
        get<2>(res) = nodes;
        return res;
    } else {
        throw runtime_error("Could not open file");
    }
}
