# CS3339_TermProject

# Mips Simulator

Work done by Joseph Mancillas, Eduardo Reyes, Manuel Salinas

Implementation of a 8 stage mips pipeline with forwarding and data caching implemented. This implementation is based off 
prior work done by Lee B. Hinkle which is based on earlier work by Martin Burtscher and Molly O'Neil. We would like to 
thank them for giving us a suitable starting point for this project. Using the provided baseline fles we were able to 
implement data cache and forwarding system to the mips architecture.

# State machine diagram of cache implementation 
![](https://github.com/josephmancillas/CS3339_TermProject/blob/master/images/cache%20state%20machine.png)

Source: pg. 405 Computer Organization and Design by David Patterson and John Hennessy

Using this state machine diagram we attempted to replicate a 8 stage mips pipeline that includes forwarding paths, 
branching, hazard detection and on which memory operation the hazard occured on. This calculation is determined overall as 
the number of individual hazards on a particular memory operation but also as a percentage of all that occured on that 
stage of the pipeline 

The implemented cache simulator calculates the number of load and store misses. This cache simulator also provides the 
number of accesses overall for a program and will calculate writebacks and hit ratio according to the number of accesses to 
the cache. 

Other statistics shown are the hex address at which the program counter finished at. The number of cycles that it took to 
complete the overall program. Underneath cycles displays the average CPI (cycles per instruction) that occured to complete 
one instruction.

# Before running: 
All input files which are mips executables are located in the input directory. These files should only be used.
loading .asm or using other programs to dump a .asm to convert to binary or machine code may cause incorrect output
to occur. 

# Compilation Procedures

Open Terminal and cd into directory

make clean //Remove any old object files that may cause eronious output

With all .h and .cpp files in same directory 

compilation is as follows

make 

To run the simulator executable

./simulator ./input "mips executable file" All input files are located within the input directory

# Example run procedure

./simulator input/rand.mips

