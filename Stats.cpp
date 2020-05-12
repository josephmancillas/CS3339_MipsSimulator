
/******************************
 * Work done by Joseph Mancillas Net ID: j_m1083
 * Updated in Spring 2020 with work done by: Joseph Mancillas, Eduardo Reyes, Manuel Salinas
 * CS 3339 - Fall 2019, Texas State University
 * Project 5: Data Cache
 * Copyright 2019, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/

#include "Stats.h"

Stats::Stats() {
  cycles = PIPESTAGES - 1; // pipeline startup cost
  flushes = 0;
  bubbles = 0;

  memops = 0;
  branches = 0;
  taken = 0;
  stalls = 0;

  exe1Count = 0;
  exe2Count = 0;
  mem1Count = 0;
  mem2Count = 0;
  hazardCount = 0;

  for(int i = IF1; i < PIPESTAGES; i++) {
    resultReg[i] = -1;
    resultStage[i] = ID;
  }
}

/*
  Advances the pipeline foward
*/
void Stats::clock() {
  cycles++;

  // advance all pipeline flip-flops
  for(int i = WB; i > IF1; i--) {
    resultReg[i] = resultReg[i-1];
    resultStage[i] = resultStage[i-1];
  }
  // inject a no-op into IF1
  resultReg[IF1] = -1;
  resultStage[IF1] = ID;
}
/*
  Takes in the the register source for the pipeline and determines where the 
  hazards are and how many times the pipeline will need to bubble
*/

void Stats::registerSrc(int r, PIPESTAGE stage) {

int bubblesNeeded = 0;
bool hazardFound = false;

if (r == 0)
  return;
for (int currentReg = EXE1; currentReg  < WB; currentReg++){
  if (r == resultReg[currentReg]) 
  {
    if (hazardFound == false) {
      switch (currentReg){
      case EXE1:
        exe1Count++;
        hazardFound = true;
        break;
      case MEM1:
        mem1Count++;
        hazardFound = true;
        break;
      case EXE2:
        exe2Count++;
        hazardFound = true;
        break;
      case MEM2:
        mem2Count++;
        hazardFound = true;
        break;
      default: 
	      hazardFound = false;
	      break;
    }
  }
    int availableIn = resultStage[currentReg] - currentReg;
    int neededIn = stage - ID;

    // Calculation to determine how many bubbles will need to be added to make the next register available
    bubblesNeeded =  availableIn - neededIn;

    for (int j = 0; j < bubblesNeeded; j++){
      bubble();
      }

    }
  }
}

/*
  Determines where the result register is needed and 
  where the result stage is within the pipeline
*/

void Stats::registerDest(int r, PIPESTAGE stage) {
  resultReg[ID] = r;
  resultStage[ID] = stage;  
}

/*
  Flushes the pipeline and clears hazards
*/

void Stats::flush(int count) { 
  for (int i = 0; i < count; i++){
    flushes++;
    clock();
  }
}

/*
  Advances the pipeline and injects a noop into the pipeline
*/

void Stats::bubble() { //DO NOT TOUCH
  //Run flops forward of ID, inject -1 to EXE1
  bubbles++;
  cycles++;
  

  for (int i = WB; i > ID; i--) {
    resultReg[i] = resultReg[i - 1];
    resultStage[i] = resultStage[i - 1];
  }

  resultReg[EXE1] = -1;
  resultStage[EXE1] = ID;
}

/*
  Takes in a the a int perameter and determines how many times the pipeline will need to be stalled
*/

void Stats::stall(int timesToStall){
  stalls += timesToStall;
  cycles += timesToStall;

}
