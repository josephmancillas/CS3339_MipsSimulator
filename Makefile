CFLAGS=-O3 -std=c++11

ifeq ($(CACHE_EN), 0)
	CFLAGS+=-DCACHE_EN=$(CACHE_EN)
endif
ifdef BLOCKSIZE
	CFLAGS+=-DBLOCKSIZE=$(BLOCKSIZE)
endif
ifdef SETS
	CFLAGS+=-DSETS=$(SETS)
endif
ifdef WAYS
	CFLAGS+=-DWAYS=$(WAYS)
endif
ifdef LOOKUP_LATENCY
	CFLAGS+=-DLOOKUP_LATENCY=$(LOOKUP_LATENCY)
endif
ifdef READ_LATENCY
	CFLAGS+=-DREAD_LATENCY=$(READ_LATENCY)
endif
ifdef WRITE_LATENCY
	CFLAGS+=-DWRITE_LATENCY=$(WRITE_LATENCY)
endif
ifdef VICTIM_BUFFER
	CFLAGS+=-DVICTIM_BUFFER
endif

simulator: ALU.o CPU.o Memory.o Stats.o CacheStats.o Simulator.o
	g++ $(CFLAGS) ALU.o CPU.o Memory.o Stats.o CacheStats.o Simulator.o -o simulator

ALU.o: Debug.h ALU.h ALU.cpp
	g++ $(CFLAGS) -c ALU.cpp

CacheStats.o: Debug.h CacheStats.h CacheStats.cpp
	g++ $(CFLAGS) -c CacheStats.cpp

CPU.o: Debug.h ALU.h Memory.h Stats.h CacheStats.h CPU.h CPU.cpp
	g++ $(CFLAGS) -c CPU.cpp

Memory.o: Debug.h Memory.h Memory.cpp
	g++ $(CFLAGS) -c Memory.cpp

Stats.o: Debug.h Stats.h Stats.cpp
	g++ $(CFLAGS) -c Stats.cpp

Simulator.o: Debug.h CPU.h Memory.h Stats.h CacheStats.h Simulator.cpp
	g++ $(CFLAGS) -c Simulator.cpp

.PHONY: clean
clean:
	rm -f ALU.o CacheStats.o CPU.o Memory.o Stats.o Simulator.o simulator
