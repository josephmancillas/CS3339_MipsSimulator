/******************************
 * Work done by Joseph Mancillas j_m1083 
 * Updated in Spring 2020 with work done by Joseph Mancillas, Eduardo Reyes, Manuel Salinas
 * CS 3339 - Fall 2019, Texas State University
 * Project 5 Data Cache
 * Copyright 2019, all rights reserved
 * Updated by Lee B. Hinkle based on prior work by Martin Burtscher and Molly O'Neil
 ******************************/

#include "CPU.h"
#include "Stats.h"
#include "CacheStats.h"



Stats stats;
CacheStats cache;


const string CPU::regNames[] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3",
                               "$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                               "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7",
                               "$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};


/*
    Function CPU() by professor Lee B Hinkle
*/

CPU::CPU(uint32_t pc, Memory &iMem, Memory &dMem) : pc(pc), iMem(iMem), dMem(dMem) {

   for(int i = 0; i < NREGS; i++) {
       regFile[i] = 0;
   }
   hi = 0;
   lo = 0;
   regFile[28] = 0x10008000; // gp
   regFile[29] = 0x10000000 + dMem.getSize(); // sp

   instructions = 0;
   stop = false;
}

/*
  Function run() by professor Lee B Hinkle.
  Update to clock function with modified stats call by Joseph Mancillas.
*/
void CPU::run() {

   while(!stop) {
       instructions++;

       fetch();
       decode();
       execute();
       mem();
       writeback();

       stats.clock();
   }
}

/*
    Function fetch() by professor Lee B Hinkle
*/

void CPU::fetch() {
   instr = iMem.loadWord(pc);
   pc = pc + 4;
}

/////////////////////////////////////////
// ALL YOUR CHANGES GO IN THIS FUNCTION
/////////////////////////////////////////


void CPU::decode() {

   uint32_t opcode;      // opcode field
   uint32_t rs, rt, rd;  // register specifiers
   uint32_t shamt;       // shift amount (R-type)
   uint32_t funct;       // funct field (R-type)
   uint32_t uimm;        // unsigned version of immediate (I-type)
   int32_t simm;         // signed version of immediate (I-type)
   uint32_t addr;        // jump address offset field (J-type)

   opcode = (instr >> 26) & 0x3f;
   rs = (instr >> 21) & 0x1f;
   rt = (instr >> 16) & 0x1f;
   rd = (instr >> 11) & 0x1f;
   shamt = (instr >> 6) & 0x1f;
   funct = instr & 0x3f;
   uimm = instr & 0xffff;
   simm = ((signed)instr << 16) >> 16;
   addr = instr & 0x3ffffff;

   aluOp = ADD;
   writeDest = false;
   destReg = REG_ZERO;
   opIsMultDiv = false;
   opIsLoad = false;
   opIsStore = false;
   aluSrc1 = REG_ZERO;
   aluSrc2 = REG_ZERO;
   storeData = REG_ZERO;

   D(cout << "  " << hex << setw(8) << pc - 4 << ": ");
   switch(opcode)
   {
       case 0x00:
           switch(funct)
           {
               case 0x00: D(cout << "sll " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                   aluOp = SHF_L;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = shamt;
                   break;
               case 0x03: D(cout << "sra " << regNames[rd] << ", " << regNames[rs] << ", " << dec << shamt);
                   aluOp = SHF_R;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = shamt;
                   break;
               case 0x08: D(cout << "jr " << regNames[rs]);
                   pc = (pc & 0xf0000000) | regFile[rs]; stats.flush(2); stats.registerSrc(rs, ID);
                   break;
               case 0x10: D(cout << "mfhi " << regNames[rd]);
                   aluOp = ADD;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = hi; stats.registerSrc(REG_HILO, EXE1);
                   aluSrc2 = REG_ZERO;
                   break;
               case 0x12: D(cout << "mflo " << regNames[rd]);
                   aluOp = ADD;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = lo; stats.registerSrc(REG_HILO, EXE1);
                   aluSrc2 = REG_ZERO;
                   break;
               case 0x18: D(cout << "mult " << regNames[rs] << ", " << regNames[rt]);
                   aluOp = MUL;
                   opIsMultDiv = true; destReg = REG_HILO; stats.registerDest(REG_HILO, WB);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
               case 0x1a: D(cout << "div " << regNames[rs] << ", " << regNames[rt]);
                   aluOp = DIV;
                   opIsMultDiv = true; destReg = REG_HILO; stats.registerDest(REG_HILO, WB);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
               case 0x21: D(cout << "addu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   aluOp = ADD;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
               case 0x23: D(cout << "subu " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   aluOp = ADD;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = -1 * (signed)regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
               case 0x2a: D(cout << "slt " << regNames[rd] << ", " << regNames[rs] << ", " << regNames[rt]);
                   aluOp = CMP_LT;
                   writeDest = true; destReg = rd; stats.registerDest(rd, MEM1);
                   aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
                   aluSrc2 = regFile[rt]; stats.registerSrc(rt, EXE1);
                   break;
               default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
           }
           break;
       case 0x02: D(cout << "j " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
           pc = (pc & 0xf0000000) | addr << 2; stats.flush(2);
           break;
       case 0x03: D(cout << "jal " << hex << ((pc & 0xf0000000) | addr << 2)); // P1: pc + 4
           writeDest = true; destReg = REG_RA; stats.registerDest(REG_RA, EXE1);
           aluOp = ADD;
           aluSrc1 = pc;
           aluSrc2 = regFile[REG_ZERO];
           pc = (pc & 0xf0000000) | addr << 2; stats.flush(2);
           break;
       case 0x04: D(cout << "beq " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
           //writeDest = false;
           //aluOp = ADD;

           stats.registerSrc(rs, ID);
           stats.registerSrc(rt, ID);

           if(regFile[rs] == regFile[rt])
           {
               pc = pc + (simm << 2);
               stats.flush(2);
               stats.countTaken();
           }
           stats.countBranch();

           break;
       case 0x05: D(cout << "bne " << regNames[rs] << ", " << regNames[rt] << ", " << pc + (simm << 2));
          // writeDest = false;
           //aluOp = ADD;

           stats.registerSrc(rs, ID);
           stats.registerSrc(rt, ID);

           if(regFile[rs] != regFile[rt]){
               pc = pc + (simm << 2);
               stats.flush(2);
               stats.countTaken();
           }

           stats.countBranch();

           break;
       case 0x09: D(cout << "addiu " << regNames[rt] << ", " << regNames[rs] << ", " << dec << simm);
           writeDest = true; destReg = rt; stats.registerDest(rt, MEM1);
           aluOp = ADD;
           aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
           aluSrc2 = simm;
           break;
       case 0x0c: D(cout << "andi " << regNames[rt] << ", " << regNames[rs] << ", " << dec << uimm);
           writeDest = true; destReg = rt; stats.registerDest(rt, MEM1);
           aluOp = AND;
           aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
           aluSrc2 = uimm;
           break;
       case 0x0f: D(cout << "lui " << regNames[rt] << ", " << dec << simm);
           writeDest = true; destReg = rt; stats.registerDest(rt, MEM1);
           aluOp = SHF_L;
           aluSrc1 = simm;
           aluSrc2 = 16;
           break;
       case 0x1a: D(cout << "trap " << hex << addr);
           switch(addr & 0xf)
           {
               case 0x0: cout << endl; break;
               case 0x1: cout << " " << (signed)regFile[rs]; stats.registerSrc(rs, EXE1);
                   break;
               case 0x5: cout << endl << "? "; cin >> regFile[rt]; stats.registerDest(rt, MEM1);
                   break;
               case 0xa: stop = true;
                   break;
               default: cerr << "unimplemented trap: pc = 0x" << hex << pc - 4 << endl;
                   stop = true;
           }
           break;
       case 0x23: D(cout << "lw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
           writeDest = true; destReg = rt; stats.registerDest(rt, WB);
           opIsLoad = true;
           aluOp = ADD;
           aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
           aluSrc2 = simm;
           stats.countMemOp();
           break;
       case 0x2b: D(cout << "sw " << regNames[rt] << ", " << dec << simm << "(" << regNames[rs] << ")");
           writeDest = false;
           aluOp = ADD;
           opIsStore = true;
           aluSrc1 = regFile[rs]; stats.registerSrc(rs, EXE1);
           aluSrc2 = simm;
           storeData = regFile[rt]; stats.registerSrc(rt, MEM1);
           stats.countMemOp();
           break;
       default: cerr << "unimplemented instruction: pc = 0x" << hex << pc - 4 << endl;
   }
   D(cout << endl);
}

/*
    Function execute by professor Lee B Hinkle
*/

void CPU::execute()
{
   aluOut = alu.op(aluOp, aluSrc1, aluSrc2);
}

/*
    Function mem() by professor Lee B Hinkle
    Calls to cache.access() by Joseph Mancillas
*/

void CPU::mem()
{
   if(opIsLoad) {
       writeData = dMem.loadWord(aluOut);
       cache.access(aluOut, LOAD);
   }
   else
    writeData = aluOut;


   if(opIsStore) {
     dMem.storeWord(storeData, aluOut);
     cache.access(aluOut, STORE);
   }
}

/*
  Function writeback() by professor Lee B Hinkle
*/

void CPU::writeback()
   {
       if (writeDest && destReg > 0) // skip when write is to zero register
           regFile[destReg] = writeData;

       if (opIsMultDiv)
       {
           hi = alu.getUpper();
           lo = alu.getLower();
       }
   }

/*
  Function PrintRegFile() by professor Lee B Hinkle
*/

void CPU::printRegFile()
{
   cout << hex;
   for(int i = 0; i < NREGS; i++)
   {
       cout << "    " << regNames[i];
       if(i > 0) cout << "  ";
       cout << ": " << setfill('0') << setw(8) << regFile[i];
       if( i == (NREGS - 1) || (i + 1) % 4 == 0 )
           cout << endl;
   }
   cout << "    hi   : " << setfill('0') << setw(8) << hi;
   cout << "    lo   : " << setfill('0') << setw(8) << lo;
   cout << dec << endl;
}

void CPU::printFinalStats()
{
   cout << "Program finished at pc = 0x" << hex << pc << "  ("
        << dec << instructions << " instructions executed)" << endl
        << endl;

   cout << "Cycles: " << stats.getCycles() + cache.cache_stats.getCycles() << endl
   << "CPI: " << fixed << setprecision(2) << 1.0 * (stats.getCycles() + cache.cache_stats.getCycles())/ instructions
   << endl
   << endl
   << "Bubbles: " << stats.getBubbles() + cache.cache_stats.getBubbles() << endl
   << "Flushes: " << stats.getFlushes() + cache.cache_stats.getFlushes() << endl
   << "Stalls:  " << stats.getStallCount() + cache.cache_stats.getStallCount() << endl
   << endl;

   cout << "RAW hazards: " << stats.getHazards() << " (1" << " per every " << fixed << setprecision(2)
        << 1.0*instructions/stats.getHazards() << " instructions)"
        << endl;

   cout << fixed << setprecision(0)
        << "  On EXE1 op: " << stats.getExe1Count() << " (" << 100.0*stats.getExe1Count()/stats.getHazards() << "%)" << endl
        << "  On EXE2 op: " << stats.getExe2Count() << " (" << 100.0*stats.getExe2Count()/stats.getHazards() << "%)" << endl
        << "  On MEM1 op: " << stats.getMem1Count() << " (" << 100.0*stats.getMem1Count()/stats.getHazards() << "%)" << endl
        << "  On MEM2 op: " << stats.getMem2Count() << " (" << 100.0*stats.getMem2Count()/stats.getHazards() << "%)"
        << endl << endl;

    cache.printFinalStats();

}
