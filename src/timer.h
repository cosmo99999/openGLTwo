#pragma once
#include <chrono>
#include <ctime>
#include <cmath>

class Timer {
public:
  Timer(){};
  void Start(){
    startTime = std::chrono::system_clock::now();
  }
  double ElapsedMilliseconds(){
    endTime = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
  }
  

private:
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::time_point<std::chrono::system_clock> endTime;
  
};
