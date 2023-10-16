#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "stub.h"

#define CLIENTS 2

int num_clients = 3;

int main(int argc, char *argv[]) {
    int estado = 0;

    char *ip = argv[1];
    // Get the port
    short port = atoi(argv[2]);

    
    estado = initServer(ip, port, "P2", CLIENTS);
    if (estado == -1) {
        fprintf(stderr, "Error when calling init\n");
        exit(EXIT_FAILURE);
    }

    
    int exit = 0;

    while (exit == 0) {
        if (GetClockLamport() == num_clients) {
            exit = 1;
        }
     
        // Server process the clients
        ProcessPetitions(thread_server);
    }
    
    for (int i = 0; i < CLIENTS; i++) {
        send_message(1, "P2");
        // terminar
    }

}