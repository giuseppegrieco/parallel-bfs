### parallel-bfs/scripts
# Scripts
## Setup
This code requires Python 3.7 or later, then you need to install the dependencies in order to run the script:
```
cd data_collection
pip install -r requirements.txt
```
## Usage
### `common parameters`:
- `numNodes`: number of nodes to create
- `start`: starting node
- `labelMin`: labels minimum value
- `labelMax`: labels maximum value
- `target`: label target
- `outputFile`: the path to the output files
- `--seed`: seed for the generation of the graph
### `gen_erdos_renyi.py`:
It generates random DAG using Erdos-Renyi model.

usage: python `gen_erdos_renyi.py` [-h] [--seed SEED] numNodes start labelMin labelMax target outputFile p:  
- `p`: probability of attaching one edge
### `gen_barabasi_albert.py`
It generates random DAG using Barabasi-Albert model

usage: python `gen_erdos_renyi.py` [-h] [--seed SEED] umNodes start labelMin labelMax target outputFile m:  
- `m`: number of edges to attach from a new node to existing nodes
### `gen_watts_strogatz.py`
It generates random DAG using Watts-Strogatz model

usage: python `gen_watts_strogatz.py` [-h] [--seed SEED] numNodes start labelMin labelMax target outputFile p k
- `p`: probability of attaching a new edge for each edge
- `k`: number of node to which each node is connected in ring topology