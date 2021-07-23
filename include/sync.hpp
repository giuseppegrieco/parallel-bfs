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
 * It allows atomically retrieves a chunk
 * for a given frontier.
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

    /**
     * @return the next chunk to process.
     */
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

    /**
     * It resets the variables according to the next frontier.
     * 
     * @param nextLevel is a pointer to the next level frontier
     */
    void setFrontier(vector<uint> *nextLevel) {
        frontier = nextLevel;
        s = 0;
        e = frontier->size();
    }
};

/**
 * It implements the level-synchronization
 * mechanism
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
    
    /**
     * It waits until all the workers 
     * terminated the process of the frontier.
     */
    void waitWorkers() {
        while(this->wCounter != nw - 1);
    }

    /**
     * It resets the counter of the workers.
     */
    void reset() {
        this->wCounter = 0;
    }

    /**
     * It increments the counter of workers.
     */
    void increment() {
        this->wCounter++;
    }

    /**
     * It waits until all the workers and the master
     * terminated the process of the frontier.
     */
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