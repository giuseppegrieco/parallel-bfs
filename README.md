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
│   └── utility script for generating random dag and benchmarking (.py)
├── Makefile
```
## Building

The project build requires c++17 and FastFlow and uses Makefile:

``` bash
> make
```

## Usage
positional arguments:
- `inputFile` : string, the path to the graph
- `startingNodeId` : integer, the id of the from which the bfs will start
- `labelTarget` : integer, label whose occurrences are to be counted
- `nw` : integer, the number of workers to use
- `k` : integer, chunk size

``` bash
> ./build/bfs-sequential inputFile startingNodeId labelTarget
```
``` bash
> ./build/bfs-pthread-static inputFile startingNodeId labelTarget nw
```
``` bash
> ./build/bfs-pthread-dynamic inputFile startingNodeId labelTarget nw k
```
``` bash
> ./build/bfs-fastflow inputFile startingNodeId labelTarget nw k
```