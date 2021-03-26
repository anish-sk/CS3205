-----Readme-----

CS3205 Assignment 2
CS18B050 - Aniswar Srivatsa Krishnan

Instructions to compile
1)  Run make in the directory of the assignment
    This will run the following commands
	    g++ -std=c++11 cw.cpp -o cw
    and generate the executable cw corresponding to the simulation program.

Instructions to run
1) Suppose we have the following parameters, K_i = 1, K_m = 1, K_n = 1, K_f = 0.1, Ps = 0.01, T = 2000, outfile = output. Run the program as follows:
    ./cw -i 1 -m 1 -n 1 -f 0.1 -T 2000 -s 0.01 -o output
2) The program will run and two directories are created: outputs and plots.
The output is stored in the file outputs/output and the graph is stored in the file plots/output.png. The program internally calls plot.py for generating the plots.
3) Run make clean to remove the executable

Instructions to generate the outputs and plots for the 32 parameter combinations mentioned in the problem statement.
1) The bash script script.sh accomplishes this task
2) Run the script as :
    ./script.sh
3) The program will run and two directories are created: outputs and plots.
4) For a particular parameter combination, the output file will be saved at outputs/{K_i}_{K_m}_{K_n}_{K_f}_{P_s} and the plot will be saved at plots/{K_i}_{K_m}_{K_n}_{K_f}_{P_s}.png 
