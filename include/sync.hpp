#include <iostream>
#include <mutex>
#include <condition_variable>
#include <deque>
#include <vector>
#include <chrono>
#include <cstddef>
#include <math.h>
#include <string>

struct task {
    queue<uint> frontier;
    bool none = false;
};

class TaskQueue {
private:
    std::mutex d_mutex;
    std::condition_variable d_condition;
    std::deque<task> d_queue;
    task none;
public:
    TaskQueue() {
        this->none.none = true;
    }

    bool empty() {
        return this->d_queue.empty();
    }

    task pop() {
        std::unique_lock<std::mutex> lock(this->d_mutex);
        if(!this->d_queue.empty()) {
            task rc(std::move(this->d_queue.back()));
            this->d_queue.pop_back();
            return rc;
        } else {
            return this->none;
        }
    }

    void add(task t) {
        {
            std::unique_lock<std::mutex> lock(this->d_mutex);
            d_queue.push_front(t);
        }
        this->d_condition.notify_one();
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
        while(this->counter != 0); 
    }

    void increment() {
        this->counter++;
    }

    void reset() {
        this->counter = 0;
    }
};
