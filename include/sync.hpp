#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <cstddef>
#include <math.h>
#include <string>

using Chunk = pair<uint, uint>;

class FeedbackQueue {
private:
    vector<uint> *frontier;
public:
    FeedbackQueue() {
        this->frontier = new vector<uint>();
    }

    vector<uint>* get() {
        return this->frontier;
    }

    void add(uint el) {
        frontier->push_back(el);
    }

    void clear() {
        this->frontier = new vector<uint>();
    }
};

class TaskQueue {
private:
    vector<vector<uint>> subfrontier;
    vector<uint> *frontier;
    atomic_uint s;
    uint e;
    uint k;
    pair<uint, uint> none;
public:
    TaskQueue(uint k) {
        this->k = k;
        this->none.second = 0;
    }

    vector<uint>* get() {
        return frontier;
    }

    pair<uint, uint> pop() {
        uint i = s.fetch_add(k);
        if(i < e) {
            pair<uint, uint> chunk;
            chunk.first = i;
            chunk.second = min(i + k, e);
            return chunk;
        } else {
            return none;
        }
    }

    void setFrontier(vector<uint> *nextLevel) {
        frontier = nextLevel;
        s = 0;
        e = frontier->size();
    }
};

class LevelSynchronization {
private:
    uint nworkers;
    atomic_ulong counter;
public:
    LevelSynchronization(uint nworkers) {
        this->nworkers = nworkers;
        this->counter = 0;
    }
    
    void waitSlaves() {
        while(this->counter != nworkers - 1); 
    }

    void waitMaster() {
        while(this->counter != 0) {
            this_thread::yield();
        }
    }

    void increment() {
        this->counter++;
    }

    void reset() {
        this->counter = 0;
    }
};
