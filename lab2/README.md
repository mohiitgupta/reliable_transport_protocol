To run alernating bit protocol in unidirectional manner, kindly use #define BIDIRECTIONAL 0 

To run it in bidirectional, use #define BIDIRECTIONAL 1.

To run it, use
gcc prog2_arq.c -g -Wall -Wno-return-type -Wno-implicit-function-declaration -Wno-implicit-int -Wno-incompatible-library-redeclaration -Wno-unused-variable -o arq.out

then, run

./arq.out

Sample run will look like:
Input:
Enter the number of messages to simulate: 1000
Enter  packet loss probability [enter 0.0 for no loss]:0.1
Enter packet corruption probability [0.0 for no corruption]:0.2
Enter average time between messages from sender's layer5 [ > 0.0]:50
Enter TRACE:2

Output:
Simulation Results:
Protocol: Alternating Bit Protocol
484 of packets sent from the Application Layer of Sender A
917 of packets sent from the Transport Layer of Sender A
1245 packets received at the Transport layer
381 packets received at the Application layer
Total time: 51098.773438 time units
Throughput = 0.007456 packets/time units


To run Go back N protocol, use:
o run it, use
gcc prog2_gbn.c -g -Wall -Wno-return-type -Wno-implicit-function-declaration -Wno-implicit-int -Wno-incompatible-library-redeclaration -Wno-unused-variable -o gbn.out

then, run
./gbn.out


Sample run will look like:
Input:
Enter the number of messages to simulate: 1000
Enter  packet loss probability [enter 0.0 for no loss]:0.1
Enter packet corruption probability [0.0 for no corruption]:0.2
Enter average time between messages from sender's layer5 [ > 0.0]:50
Enter TRACE:2

Simulation Results:
Protocol: Go-Back-N
1000 of packets sent from the Application Layer of Sender A
2628 of packets sent from the Transport Layer of Sender A
2375 packets received at the Transport layer
999 packets received at the Application layer
Total time: 50696.601562 time units
Throughput = 0.019705 packets/time units



To run selective repeat, kindly compile using below:
gcc prog2_sr.c -g -Wall -Wno-return-type -Wno-implicit-function-declaration -Wno-implicit-int -Wno-incompatible-library-redeclaration -Wno-unused-variable -o sr.out

then, run the generated sr.out using:

./sr.out

Sample run will look like below:

Input:
Enter the number of messages to simulate: 1000
Enter  packet loss probability [enter 0.0 for no loss]:0.6
Enter packet corruption probability [0.0 for no corruption]:0.2
Enter average time between messages from sender's layer5 [ > 0.0]:50
Enter TRACE:2

Output:

Simulation Results:
Protocol: Selective Repeat
1000 of packets sent from the Application Layer of Sender A
9191 of packets sent from the Transport Layer of Sender A
3815 packets received at the Transport layer
998 packets received at the Application layer
Total time: 49321.230469 time units
Throughput = 0.020235 packets/time units




