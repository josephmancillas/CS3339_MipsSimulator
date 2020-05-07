#ifndef __STATS_H
#define __STATS_H
#include "Debug.h"
#include <iostream>

using namespace std;

enum PIPESTAGE { IF1 = 0, IF2 = 1, ID = 2, EXE1 = 3, EXE2 = 4, MEM1 = 5, 
                 MEM2 = 6, WB = 7, PIPESTAGES = 8 };

class Stats {
  private:
    long long cycles;
    int flushes;
    int bubbles;
  
    int memops;
    int branches;
    int taken;

    int stalls;
    int exe1Count;
    int exe2Count; 
    int mem1Count;
    int mem2Count;
    int hazardCount;

    int resultReg[PIPESTAGES];
    int resultStage[PIPESTAGES];

  public:
    Stats();

    void clock();

    void flush(int count);

    void registerSrc(int r, PIPESTAGE stage);
    void registerDest(int r, PIPESTAGE stage);

    void countMemOp() { memops++; }
    void countBranch() { branches++; }
    void countTaken() { taken++; }
    void stall(int timesToStall);

    // getters
    long long getCycles() { return cycles; }
    int getFlushes() { return flushes; }
    int getBubbles() { return bubbles; }
    int getMemOps() { return memops; }
    int getBranches() { return branches; }
    int getTaken() { return taken; }
    int getExe1Count() { return exe1Count; }
    int getExe2Count() { return exe2Count; }
    int getMem1Count(){ return mem1Count; }
    int getMem2Count() { return mem2Count; }
    int getStallCount() { return stalls; }
    int getHazards() { return getExe1Count() + getExe2Count()
                              + getMem1Count() + getMem2Count(); }
  private:
    void bubble();
};

#endif
