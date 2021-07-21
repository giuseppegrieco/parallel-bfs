# 
# FF_ROOT     pointing to the FastFlow root directory (i.e.
#             the one containing the ff directory).
ifndef FF_ROOT
FF_ROOT		= $(HOME)/fastflow
endif

CXX = g++ -std=c++17 
CXXFLAGS = -O3 -Wall -ftree-vectorize
INCLUDE = -I include/ 
PTHREAD = -pthread
FF = -I $(FF_ROOT) 

all:
	mkdir ./build -p
	@echo "Compiling sequential version"
	$(CXX) ./src/bfs-sequential.cpp -o ./build/bfs-sequential $(CXXFLAGS) $(INCLUDE)
	@echo "Compiled ./build/bfs-sequential"
	@echo "Compiling pthread version"
	$(CXX) ./src/bfs-pthread.cpp -o ./build/bfs-pthread $(CXXFLAGS) $(INCLUDE) $(PTHREAD)
	@echo "Compiled ./build/bfs-pthread"
	@echo "Compiling fastflow version"
	$(CXX) ./src/bfs-fastflow.cpp -o ./build/bfs-fastflow $(CXXFLAGS) $(INCLUDE) $(PTHREAD) $(FF)
	@echo "Compiled ./build/bfs-fastflow"

TEST:
	mkdir ./build -p
	@echo "Compiling sequential version"
	$(CXX) ./src/bfs-sequential.cpp -o ./build/bfs-sequential-test $(CXXFLAGS) $(INCLUDE) -DTEST
	@echo "Compiled ./build/bfs-sequential-test"
	@echo "Compiling pthread version"
	$(CXX) ./src/bfs-pthread.cpp -o ./build/bfs-pthread-test $(CXXFLAGS) $(INCLUDE) $(PTHREAD) -DTEST
	@echo "Compiled ./build/bfs-pthread-test"
	@echo "Compiling fastflow version"
	$(CXX) ./src/bfs-fastflow.cpp -o ./build/bfs-fastflow-test $(CXXFLAGS) $(INCLUDE) $(PTHREAD) $(FF) -DTEST
	@echo "Compiled ./build/bfs-fastflow-test"

I_TIMER:
	@echo "Compiling sequential version with DI_TIMER"
	$(CXX) ./src/bfs-sequential.cpp -o ./build/bfs-sequential-i-timer $(CXXFLAGS) $(INCLUDE) -DI_TIMER -DTEST
	@echo "Compiled ./build/bfs-sequential-i-timer"
TIMER:
	@echo "Compiling sequential version with DTIMER"
	$(CXX) ./src/bfs-sequential.cpp -o ./build/bfs-sequential-timer $(CXXFLAGS) $(INCLUDE) -DTIMER -DTEST
	@echo "Compiled ./build/bfs-sequential-timer"
	@echo "Compiling pthread version with -DTIMER"
	$(CXX) ./src/bfs-pthread.cpp -o ./build/bfs-pthread-timer $(CXXFLAGS) $(INCLUDE) $(PTHREAD) -DTIMER -DTEST
	@echo "Compiled ./build/bfs-pthread-timer"
W_TIMER:
	@echo "Compiling pthread version with -DW_TIMER"
	$(CXX) ./src/bfs-pthread.cpp -o ./build/bfs-pthread-w-timer $(CXXFLAGS) $(INCLUDE) $(PTHREAD) -DW_TIMER -DTEST
	@echo "Compiled ./build/bfs-pthread-w-timer"
NODE_TIMER:	
	@echo "Compiling pthread version with -DNODE_TIMER"
	$(CXX) ./src/bfs-pthread.cpp -o ./build/bfs-pthread-node-timer $(CXXFLAGS) $(INCLUDE) $(PTHREAD) -DNODE_TIMER -DTEST
	@echo "Compiled ./build/bfs-pthread-node-timer"
VISITED_TIMER:
	@echo "Compiling pthread version with -DVISITED_TIMER"
	$(CXX) ./src/bfs-pthread.cpp -o ./build/bfs-pthread-visited-timer $(CXXFLAGS) $(INCLUDE) $(PTHREAD) -DVISITED_TIMER -DTEST
	@echo "Compiled ./build/bfs-pthread-visited-timer"

.PHONY: clean
clean: 
	@echo "Deleting build"
	rm -r ./build
	@echo "Build deleted"
