#ifndef __CACHE_STATS_H
#define __CACHE_STATS_H

#include <cstdint>
#include "Debug.h"
#include "Stats.h"
#include "CPU.h"

using namespace std;

#ifndef CACHE_EN
#define CACHE_EN 1
#endif

#ifndef BLOCKSIZE
#define BLOCKSIZE 32
#endif

#ifndef SETS
#define SETS 8
#endif

#ifndef WAYS
#define WAYS 4
#endif

#ifndef LOOKUP_LATENCY
#define LOOKUP_LATENCY 0
#endif

#ifndef READ_LATENCY
#define READ_LATENCY 30
#endif

#ifndef WRITE_LATENCY
#define WRITE_LATENCY 10
#endif

enum ACCESS_TYPE { LOAD, STORE };

class CacheStats {
  private:
    /* TODO: you probably want to add some member variables here to represent
     * the parts of the cache contents you need to model! */
    uint32_t tag;
    uint32_t index;

    uint32_t tags[SETS][WAYS];
    bool valid[SETS][WAYS];
    bool dirty[SETS][WAYS];
    int LRU_bit[SETS];

    bool hit;
    int r = 0;
    int way_count;
    int set_count;
    int loads = 0;
    int stores = 0;
    int load_misses = 0;
    int store_misses = 0;
    int writebacks = 0;


  public:
    Stats cache_stats;
    CacheStats();
    int access(uint32_t, ACCESS_TYPE);
    int getNextVal();
    void printFinalStats();
};

#endif
