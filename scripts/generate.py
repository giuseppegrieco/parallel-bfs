import argparse

def probability(val):
    x = float(val)
    if x >= 0 and x <= 1:
        return x
    raise ValueError(x)

parser = argparse.ArgumentParser(
    description='It generates random DAG'
)
parser.add_argument(
    'p', 
    help='probability of attaching one edge', 
    type=probability
)
parser.add_argument(
    'outputFile', 
    help='the path to the output files'
)
parser.add_argument(
    'target, 
    help='node target',
    type=int
)
args = parser.parse_args()

p = args.probability
outputFile = args.outputFile
target = args.target

'''

vector<node> generate_random_graph(int nLevels, int maxNodePerLevel) {
    if(nLevels < 1) {
        throw invalid_argument("nLevels should be greater than 0");
    }
    if(maxNodePerLevel < 1) {
        throw invalid_argument("maxNodePerLevel should be greater than 0");
    }

    // Create a map of node's label -> position of the node in vector
    map<int, int> nodePositions;

    vector<node> nodes;
    create_node(0, nodes, nodePositions);
    
    vector<int> previous;
    previous.push_back(0);
    for(int i = 0; i < nLevels; i++) {
        vector<int> current;
        for(int j = 0; j < maxNodePerLevel; j++) {
            bool toAdd = false;
            int label = nodes.size();
            for(size_t k = 0; k < previous.size(); k++) {
                srand(time(NULL));
                double p = (double) rand() / RAND_MAX;
                if(p > 0.5) {
                    toAdd = true;
                    create_node(label, nodes, nodePositions);
                    nodes[previous[k]].adj.push_back(nodePositions[label]);
                }
            }
            if (toAdd) {
                current.push_back(nodePositions[label]);
            }
        }
        previous = current;
    }
    return nodes;
}

void save_graph(vector<node> nodes, string outputPath) {
    fstream outputFile(outputPath, ios_base::out);
    for(size_t i = 0; i < nodes.size(); i++) {
        for(size_t j = 0; j < nodes[i].adj.size(); j++) {
            string in = to_string(nodes[i].label);
            string out = to_string(nodes[nodes[i].adj[j]].label);
            string line = in + "," + out;
            outputFile << line << endl;
        }
    }
    outputFile.close();
}

'''