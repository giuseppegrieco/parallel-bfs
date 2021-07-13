
# 
# FF_ROOT     pointing to the FastFlow root directory (i.e.
#             the one containing the ff directory).
ifndef FF_ROOT
FF_ROOT		= $(HOME)/fastflow
endif

CXX = g++ -std=c++17 
CXXFLAGS = -O3 -Wall
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
	$(CXX) ./src/bfs-fastflow.cpp -o ./build/bfs-fastflow $(CXXFLAGS) $(INCLUDE) $(FF)
	@echo "Compiled ./build/bfs-fastflow"


.PHONY: clean
clean: 
	@echo "Deleting build"
	rm -r ./build
	@echo "Build deleted"