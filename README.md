# CS3339_TermProject

Work done by Joseph Mancillas, Eduardo Reyes, Manuel Salinas

Implementation of a 8 stage mips pipeline with forwarding and data caching implemented. This implementation is based off prior work done by Lee B. Hinkle which is based on earlier work by Martin Burtscher and Molly O'Neil. We would like to thank them for giving us a suitable starting point for this project. Using the provided baseline fles we were able to implement data cache and forwarding system to the mips architecture.


Compilation Procedures

Open Terminal and cd into directory

make clean //Remove any old object files that may cause eronious output

With all .h and .cpp files in same directory 

compilation is as follows

make 

To run the simulator executable

All input files which are mips executables are located in the input directory

./simulator ./input "mips executable file"
