-----Readme-----

CS3205 Assignment 1
CS18B050 - Aniswar Srivatsa Krishnan

Instructions to compile
1)  Run make in the directory of the assignment
    This will run the following commands
    	gcc -o server emailserver.c
	    gcc -o client emailclient.c
    and generate the executables server and client corresponding to the server and client respectively

Instructions to run
1) Open a terminal in the assignment directory
2) Run ./server [PORT] where PORT is the port number to which you wish to bind the socket. (The default port is 8080)
3) Open another terminal in the assignment directory (Alternatively you could have added "&" to the above command to move the server process to the background)
4) Run ./client [IP] [PORT] where IP is the IP address of the server, and PORT is the same port number given in Step 2. The default IP address of the server is "127.0.0.1" or localhost. The default port number is again 8080
5)You can start giving the commands to the user input interface. The API is as defined in the problem statement.
6) You can observe the spool files in the directory MAILSERVER.
7) Make sure that the port you are using is not used by any other program so that our program can be run successfully

Run make clean to remove the executables
