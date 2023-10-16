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
#include <sys/param.h>

struct Stub stub;



int initServer(char *ip, int port, char *pc, int current_clients) {
    strcpy(stub.pc, pc);
    stub.clock_lamport = 0;
    stub.current_clients = current_clients;
    const int enable = 1;

    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = atoi(ip);
    server_addr.sin_port = port;

    strcpy(stub.pc, pc);
    stub.clock_lamport = 0;

    // TCP socket
    if ((stub.tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        fprintf(stderr, "ERROR: Failed to create socket\n");
        perror("errno");
        return -1;
    }

    if (bind(stub.tcp_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        fprintf(stderr, "ERROR: When binding\n");
        return 1;
    }
  
    if ((listen(stub.tcp_socket, BACKLOG)) == -1) {
        fprintf(stderr, "ERROR: Failed to start listening\n");
        perror("errno");
        return -1;
    }

    printf("Init server\n");

    return 0;
}

int initClient(char *ip, int port, char *pc) {
    strcpy(stub.pc, pc);
    stub.clock_lamport = 0;

    const int enable = 1;

    
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = atoi(ip);
    client_addr.sin_port = port;

    strcpy(stub.pc, pc);
    stub.clock_lamport = 0;

    // TCP socket
    if ((stub.tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) < -1) {
        fprintf(stderr, "ERROR: Failed to create socket\n");
        perror("errno");
        return -1;
    }

    if ((connect(stub.tcp_socket, (struct sockaddr *) &client_addr , sizeof(client_addr)) < 0)) {
		fprintf(stderr, "ERROR: Failed connecting to server...\n");
        return -1;
	}

    printf("Init client\n");

    return 0;
}


void update_lamport(int tiempo) {
    if (tiempo > stub.clock_lamport) {
        stub.clock_lamport = tiempo;
    }
}

int send_message(int operation, char *pc) {
    int s;
    // Prueba
    struct message* msg = (struct message*) malloc(sizeof(struct message));
    strcpy(msg->origin, pc);
    msg->action = operation;
    msg->clock_lamport = stub.clock_lamport++;
    
    
    memset(msg, 0, sizeof(struct message));
    
    if ((s = send(stub.tcp_socket, msg, sizeof(struct message), 0)) == -1) {
        fprintf(stderr, "Error when send\n");
        free(msg);
        return -1;
    }
    
    free(msg);
    return 0;
}

int receive_message(int client_socket) {
    int r;
    struct message* msg = (struct message*) malloc(sizeof(struct message));
    
    if ((r = recv(client_socket, msg, sizeof(struct message), 0)) < -1) {
        fprintf(stderr, "Error when recv\n");
        perror("Error: ");
        free(msg);
        return -1;
    }


    update_lamport(msg->clock_lamport);

    return 0;

}

int GetClockLamport() {
    return stub.clock_lamport;
}


void *thread_server(void *args) { // sacarlo de la libreria y una por fichero
    int client_socket = *(int*) args;

    int status = receive_message(client_socket);

    ///////////////
    printf("-->< %d \n", GetClockLamport());
    

    pthread_exit(NULL); // Salimos del threads
}



int ProcessPetitions(void* thread_server) {
    int current = 0;

    while (current < stub.current_clients) {
        socklen_t len = sizeof(stub.server_addr);

        
        if ((stub.clients[current] = accept(stub.tcp_socket, (struct sockaddr *) &stub.server_addr, &len) == -1)) {
            fprintf(stderr, "ERROR: Failed to accept connection\n");
            close(stub.clients[current]);
            exit(1);
        }        
        printf("Socket %d\n", stub.clients[current]);
////
  
        // Create threads for a maximum of 100 clients
        if (pthread_create(&stub.threads[current], NULL, thread_server, (void *) &stub.clients[current]) != 0) {
            fprintf(stderr, "ERROR: Failed to create thread\n");
            exit(1);
        }

        current++;
    }

    for (int i = 0; i < current; i++) {
        if (pthread_join(stub.threads[i], NULL) != 0) {
            fprintf(stderr, "ERROR: Failed to join thread\n");
            exit(1);
        }
    }

}

