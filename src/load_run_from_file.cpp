#include  "graph_utils.h"

pair<int, vector<node>> load_run_from_file(string filePath) {
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
        create_node(stoi(currentLine), nodes, nodePositions);

        // It goes through every line, i.e. every edge of the graph
        while(getline(inputFile, currentLine)) {
            int delimiterPos = currentLine.find(",");
            int in = stoi(currentLine.substr(0, delimiterPos));
            int out = stoi(currentLine.substr(delimiterPos + 1, currentLine.size()));

            // Checks if the node are already found previously, if not
            // add them to the nodes list
            create_node(in, nodes, nodePositions);
            create_node(out, nodes, nodePositions);

            nodes[nodePositions[in]].adj.push_back(
                nodePositions[out]
            );
        }

        inputFile.close();
        return {target, nodes};
    } else {
        throw runtime_error("Could not open file");
    }
}
