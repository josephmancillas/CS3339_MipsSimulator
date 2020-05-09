/******************************
 * Work by Joseph Mancillas, Eduardo Reyes, Manuel Salinas
 * CS 3339 - Spring 2020, Texas State University
 * Data Cache
 * Copyright 2019, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/

#include "CacheStats.h"
#include <cstdlib>
#include <iomanip>
#include <iostream>

using namespace std;

CacheStats::CacheStats() {
  cout << "Cache Config: ";
  if(!CACHE_EN) {
    cout << "cache disabled" << endl;
  } else {
    cout << (SETS * WAYS * BLOCKSIZE) << " B (";
    cout << BLOCKSIZE << " bytes/block, " << SETS << " sets, " << WAYS << " ways)" << endl;
    cout << "  Latencies: Lookup = " << LOOKUP_LATENCY << " cycles, ";
    cout << "Read = " << READ_LATENCY << " cycles, ";
    cout << "Write = " << WRITE_LATENCY << " cycles" << endl;
  }

  /*
    Initalizes the cache and sets the dirty and valid arrays as false which when 
    running the cache the first time will be a compulsory miss since the cache will 
    be accessed for the first time
  */

  
  for (way_count = 0; way_count < WAYS; way_count++) {
    for (set_count = 0; set_count < SETS; set_count++) {
      tags[way_count][set_count] = 0;
      dirty[way_count][set_count] = false;
      valid[way_count][set_count] = false;
      LRU_bit[set_count] = 0;
    }
  }

}

/*
  Takes unsigned 32 bit integer, and a Store or Load type to determine 
  which way the cache will be accessed. Using the logic from page 405 
  from the Computer Organization and Design textbook by David Patterson and John Hennessy
  we implemented a mod based cache.

  See additional documentation folder
*/

int CacheStats::access(uint32_t addr, ACCESS_TYPE type) {

  index = (addr >> 5) & 0x07;
  tag = (addr >> 8);

  LRU_bit[index] = getNextVal();
  r = LRU_bit[index];


  if (!CACHE_EN)
  { // cache disabled
    return (type == LOAD) ? READ_LATENCY : WRITE_LATENCY;
  }

  (type == LOAD) ? loads++ : stores++;

  if (tags[index][r] == tag && valid[index][r]) {

    hit = true;

    if (hit && type == STORE) {
      dirty[index][r] = true;
    } 
    else {
      tags[index][r] = tag;
      valid[index][r] = true;
    }
  } 
  else
    hit = false;

  if (!hit && dirty[index][r]) {
    writebacks++;
    cache_stats.stall(WRITE_LATENCY);
  }

  if (!hit && !dirty[index][r])
  {
      if (type == LOAD)
      {
        load_misses++;
        cache_stats.stall(READ_LATENCY);

        valid[index][r] = true;
        dirty[index][r] = false;
        tags[index][r] = tag;
        LRU_bit[index] = getNextVal();
      }

      if (type == STORE) {
        store_misses++;
        cache_stats.stall(READ_LATENCY);

        dirty[index][r] = true;
        valid[index][r] = true;
        tags[index][r] = tag;
        LRU_bit[index] = getNextVal();
      }

  }
  return LOOKUP_LATENCY;
}

/*
  Takes the current index passed in by the current address and and adds plus one and 
  mods by 4 to give us our Least Recently Used bit for our 4 way set associative cache
*/ 

int CacheStats::getNextVal() {
  return (LRU_bit[index] + 1) % 4;
}

/*
  Prints accessess of cache and collects final writeback counts.
*/

void CacheStats::printFinalStats() {
  
    for (int w = 0; w < SETS; w++) { 
     for (int s = 0; s < WAYS; s++) {
      if (dirty[s][w])
          writebacks++;
    }
  }

  int accesses = loads + stores;
  int misses = load_misses + store_misses;
  cout << "Accesses: " << accesses << endl;
  cout << "  Loads: " << loads << endl;
  cout << "  Stores: " << stores << endl;
  cout << "Misses: " << misses << endl;
  cout << "  Load misses:  " << load_misses << endl;
  cout << "  Store misses: " << store_misses << endl;
  cout << "Writebacks: " << writebacks << endl;
  cout << "Hit Ratio: " << fixed << setprecision(1) << 100.0 * (accesses - misses) / accesses;
  cout << "%" << endl;
}