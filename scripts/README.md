### parallel-bfs/scripts
# Scripts
## Setup
This code requires Python 3.7 or later, then you need to install the dependencies in order to run the script:
```
cd data_collection
pip install -r requirements.txt
```
## Usage
### `generate_rdag.py`:
It generates random DAG using Erdos-Renyi model.

usage: python `generate_rdag.py` [-h] [--seed SEED] numNodes p start labelMin labelMax target outputFile:  
- `numNodes`: number of nodes to create
- `p`: probability of attaching one edge
- `labelMin`: labels minimum value
- `labelMax`: labels maximum value
- `outputFile`: the path to the output files
- `--seed`: seed for the generation of the graph