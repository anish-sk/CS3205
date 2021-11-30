-----Readme-----

CS3205 Assignment 4
CS18B050 - Aniswar Srivatsa Krishnan

Instructions to compile
1)  Run make in the directory of the assignment
    This will run the following commands
        g++ -o SenderSR SenderSR.cpp -lpthread
        g++ -o ReceiverSR ReceiverSR.cpp -lpthread
        g++ -o SenderGBN SenderGBN.cpp -lpthread
        g++ -o ReceiverGBN ReceiverGBN.cpp -lpthread
    and generate the four executables SenderSR ReceiverSR SenderGBN ReceiverGBN corresponding to the programs implementing the Selective Repeat and the Go-Back-N protocols.

Instructions to run Selective Repeat
1) Suppose we have the following parameters for the Receiver, p = 12345, N = 400, e = 0.00001, B = 100. Run the receiver program as follows:
    ./ReceiverSR -p 12345 -N 400 -e 0.00001 -B 100
Run the receiver program before the sender program. Make sure that the receiver port is different from 10001 as this is used for the sender's port
2) Suppose we have the following parameters for the Sender, s = "127.0.0.1", p = 12345, L = 512, R = 10, N = 400, W = 4, n = 8, B = 100. Run the sender program as follows:
    ./SenderSR -s 127.0.0.1 -p 12345 -L 512 -R 10 -N 400 -W 4 -B 100 -n 8
3) The programs will run and output the required information. If the sender program terminates due to maximum retransmissions exceeding 10 then the receiver program needs to be terminated manually.

Instructions to run Go-Back-N
1) Suppose we have the following parameters for the Receiver, p = 12345, n = 400, e = 0.00001. Run the receiver program as follows:
    ./ReceiverSR -p 12345 -n 400 -e 0.00001
Run the receiver program before the sender program. Make sure that the receiver port is different from 10001 as this is used for the sender's port
2) Suppose we have the following parameters for the Sender, s = "127.0.0.1", p = 12345, l = 512, r = 10, n = 400, w = 4, b = 10. Run the sender program as follows:
    ./SenderSR -s 127.0.0.1 -p 12345 -l 512 -l 10 -n 400 -w 4 -b 10
3) The programs will run and output the required information. If the sender program terminates due to maximum retransmissions exceeding 5 then the receiver program needs to be terminated manually.
