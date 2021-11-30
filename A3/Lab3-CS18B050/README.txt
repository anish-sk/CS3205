-----Readme-----

CS3205 Assignment 3
CS18B050 - Aniswar Srivatsa Krishnan

Instructions to compile
1)  Run make in the directory of the assignment
    This will run the following commands
        g++ -std=c++11 ospf.cpp -o ospf -lpthread 
    and generate the executable ospf corresponding to the program implementing the OSPF routing protocol.

Instructions to run
1) Suppose we have the following parameters, id = 0, hi = 1, lsai = 5, spfi = 20, infile = input, outfile = output. Run the program as follows:
    ./ospf -i 0 -f input -o output -h 1 -a 5 -s 20
2) The program corresponding to one router will run.
3) If you have n routers in the simulation repeat step1 with id = 1,2,...n-1.
4)The routers are setup now and they can exchange packets and run the ospf routing protocol
5)The output for the ith router is stored in the file output-i.txt
6) Find the pids of the n processes and use the kill system call to terminate the processes
7)Run make clean to remove the executable

Instructions to run the protocol for n routers directly.
1) The bash script script.sh accomplishes this task
2) Run the script as :
    ./script.sh n t infile
    where n is the number of routers used in the protocol, t is the time for which the protocol needs to be run and infile is the name of the input file
3) The script will take care of running the n processes, and terminating them after t seconds.
4)The output for the ith router is stored in the file output-i.txt
