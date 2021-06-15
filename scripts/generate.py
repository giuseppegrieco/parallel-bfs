'''
This script generates a graph for a bfs search in according to the project format:

targetNode
startingNode
i,j for each pair of node connected

The graph is generated according to the Erdos-Renyi model, since it requires to be
acyclic only the upper triangle part of the adjencency matrix is considered.
'''

import argparse
import random
import time

def probability(val):
    x = float(val)
    if x >= 0 and x <= 1:
        return x
    raise ValueError(x)

parser = argparse.ArgumentParser(
    description='It generates random DAG using Erdos-Renyi model'
)
parser.add_argument(
    'p', 
    help='probability of attaching one edge', 
    type=probability
)
parser.add_argument(
    'numNodes', 
    help='number of nodes to create',
    type=int
)
parser.add_argument(
    'start', 
    help='starting node',
    type=int
)
parser.add_argument(
    'target', 
    help='node target',
    type=int
)
parser.add_argument(
    'outputFile', 
    help='the path to the output files'
)

args = parser.parse_args()
p = args.p
numNodes = args.numNodes
start = args.start
target = args.target
outputFile = args.outputFile

start_time = time.time()

# Generates an er graph of `numNodes` nodes
Adj =  [[ 0 for i in range(numNodes)] for j in range(numNodes)]
for i in range(0, numNodes):
    for j in range(0, numNodes):
        # Avoid self-loop
        if i != j:
            drawn = random.uniform(0, 1)
            if p >= drawn:
                Adj[i][j] = 1

# Get the upper part of the adj matrix of er graph and
# write the resulting graph in the output file
f = open(outputFile, 'w')
f.write(str(target) + '\n')
f.write(str(start) + '\n')
for i in range(0, numNodes):
    for j in range(i, numNodes):
        if(Adj[i][j] == 1):
            f.write(str(i) + ',' + str(j) + '\n')

print('--- Generation completed in %s  ---' % (time.time() - start_time))
