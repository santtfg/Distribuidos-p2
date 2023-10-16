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

int main(int argc, char *argv[]) {
    int estado = 0;

    char *ip = argv[1];
    // Get the port
    short port = atoi(argv[2]);


    char *pc = "P1";

    
    estado = initClient(ip, port, pc);
    if (estado == -1) {
        fprintf(stderr, "Error when calling init\n");
        exit(EXIT_FAILURE);
    }
     
    // encapsular en el stub
    send_message(0, pc); // client envia mensaje

}