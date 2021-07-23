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

/**
 * 
 */
class TaskQueue {
private:
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

/**
 * 
 */
class LevelSynchronization {
public:
    explicit LevelSynchronization(uint nw) : 
      nw(nw),
      wCounter(0),
      threshold(nw), 
      counter(nw),
      generation(0) {
    }
    
    void waitWorkers() {
        while(this->wCounter != nw - 1);
    }

    void reset() {
        this->wCounter = 0;
    }

    void increment() {
        this->wCounter++;
    }

    void wait() {
        std::unique_lock<std::mutex> lck{mtx};
        auto gen = generation;
        if (!--counter) {
            generation++;
            counter = nw;
            cv.notify_all();
        } else {
            cv.wait(lck, [this, gen] { return gen != generation; });
        }
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    uint nw;
    atomic_uint wCounter;
    uint threshold;
    uint counter;
    uint generation;
};