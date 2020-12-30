#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int main(void){

    char table[128][128];
    srand(time(NULL));

    for (int x = 0; x < 1000000; ++x){

        for (int i = 0; i < 128; ++i){
            for (int j = 0; j < 128; ++j){

                if (i == 0 || j == 0 || i == 127 || j == 127){

                    table[i][j] = '#';
                }
                else if (rand() % 2 == 1){

                    table[i][j] = '#';
                }
                else{

                    table[i][j] = ' ';
                }
            }

            for (int w = 0; w < 128; ++w){

                putc(table[i][w], stdout);
            }

            putc('\n', stdout);
        }

        sleep(2);
        system("clear");
    }

    return 0;
}

/*

basic explanation of project's stucture in the recording of a lecture 17.12 -> 1:00:00

to compile a program with a shared memory one has to add a flag -lrt
to compile a program with threads one has to add a flag -lpthread

//Use shmget, shmat etc. to create a shared memory. You use a filename, but you do not use a real file.
//It is just a handler. You can use file, but it makes your program slower. <sys/shm.h>, <sys/types.h>
//Consider shared memory with readonly parameter on the client side !

//You have to share a block of memory, so maybe It could be good to create a structure that contains all
//required info. Thanks to this you create a required, universal API.

Semaphors work for processes in kind a way like mutexes for threads.
Semaphors have names as the shared memory has. Thanks to this you will be able to call the same semaphor in many programs.
You just need to have the name saved somewhere e.g in header file and you have to sem_open() and sem_close()
in each program. Parent program can/should call sem_unilink() at the beginning to make sure that no semaphor
with this name is already running. At the end too.
Use semaphors with names. Unnamed require forking. Duch said that there is no need of forks in this project.
bullshit, with unnamed will be easier. Problem 

Calling sem_wait() decrements iterator. "Tell someone that I need an answer".
Calling sem_post() increments iterator. "Tell the boss that answer is already sent.

Server runner has to be divided into cycles. In each period every client/beast/bot can make a move.
Use semaphores to get info about moves from clients. Client has a chance to make a move, <= instead I should check each time, if there is a move in shared memory => (but after
defined period client has to return sth, even info about no move.) Move can be wrong, server is responsible for validation.
How to run this subsequently? Think it over.

After move time server changes clients positions and performs all required operations (move client to the campsite, add coins etc.).
At the end of cycle server changes info in a shared memory/memories. Thanks to this client view can be changed. Info about change in shared
memory/s can be a round number. Has to increment, even if there was no move. 

Should I create 4 shared memories and 4 semaphors? One for each client? It is written in a task desc. that clients can not have
access to other clients info.

How to get info that new client wants to join? How to manage this? 4 available programs with assigned shared memory names inside?
Stupid. Maybe counter in server program and enum or sth with names in the header file?

In the basic program beast is a thread of server program. Later beasts have to be a separate program, beast per thread.

Treasures are simply added to a table containing game map. Adding treasures as a separate thread?

Think over how to manage threads in a server. Clients, beasts, adding treasures as separate threads?

Expected that can be a class:
- beast
- bot
- client
- server

Should the server remember a previous client state because of bushes? Or do it on the client side.
Info about what to manage this? (previous state and move, flag bushes/not bushes)

To fetch keybindings just scanf/getchar + fflush stdin  will be enough?

How to keep info about dropped coins? Linked list node to keep coordinates and dropped value on the server side.

At the beginning of each client program run a loop 4 times to check if any shared memory is available.
Inside each shared memory keep a flag that informs if memory is already in use. I can use a client number field
(0 or -1 means not in use or sth). It doesn't even has to be an enum. Just check all possibilities.

Q/q key presssed by client can be passed in the same way as direction with a different enum value.

SERVER
 ^^^ 4 threads with infinite while loop and semaphors to manage clients keybindings. 
    plus mutexes to be sure that client doesn't change in the same time as server? YEAH
 ^^^ thread to manage server keybindings and the end of game

Client can change fields in shared memory, but copies are kept on the server side.
If client tries to change deaths, coins or round number server will fix this.

A new connection has to send info with the player number and values to set.
Client can't choose which free player will be.

FINAL SERVER:
1. Select the campsite coordinates and set values in the control struct, remember about mutex
2. Display the server view
3. Create threads: keybindings, clients connections, 

Threads server:

- handling connections
- handling keybinings
- main thread/handling game cycles

What is the best way to manage server termination?

*/