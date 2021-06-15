### Parallel breadth-first search
# parallel-bfs
This project contains the coding of three solutions to perform a bfs search on a dag, namely, a sequential version and two parallel versions: the former uses standard cpp thread, while the latter uses [FastFlow](https://github.com/fastflow/fastflow).

## Structure
```
├── ./build
│   └── contains the compiled files and the executable (.o, .d)
├── ./include
│   ├── all the headers files (.h)
├── ./src
│   └── all the sources files (.cpp)
├── ./scripts
│   └── utility script for generating random dag and benchmarking
├── Makefile
```
## Building

The project build requires c++17 and FastFlow and uses Makefile:

``` bash
> make
> ./build/bfs ../examples/0.dat
```

## Usage
Todo: write the usage


## Scripts
Scripts require Python 3.7 or later:
### `rdag.py`
It generates random DAG using Erdos-Renyi model.

usage: python `rdag.py` [-h] p numNodes start target outputFile:  
- `p`: probability of attaching one edge
- `numNodes`: number of nodes to create
- `start`: starting node
- `target`: node target
- `outputFile`: the path to the output files