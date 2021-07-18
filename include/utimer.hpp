#include <iostream>
#include <chrono>
#include <thread>

#include "acout.hpp"

#define START(timename) auto timename = std::chrono::system_clock::now();
#define STOP(timename,elapsed)  auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - timename).count();

class utimer {
  std::chrono::system_clock::time_point start;
  std::chrono::system_clock::time_point stop;
  std::string message; 
  using usecs = std::chrono::microseconds;
  using msecs = std::chrono::milliseconds;

private:
  long * us_elapsed;
  
public:
  utimer(const std::string m) : message(m),us_elapsed((long *)NULL) {
    start = std::chrono::system_clock::now();
  }
      
  utimer(const std::string m, long * us) : message(m),us_elapsed(us) {
    start = std::chrono::system_clock::now();
  }

  void restart() {
    start = std::chrono::system_clock::now();
  }

  long getElapsedTime() {
    stop = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = stop - start;
    return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count();
  }

  void print(string task, long musec) {
    acout() << message << " : " << task << " computed in " << musec << " nanoseconds " 
        << std::endl;
  }
};