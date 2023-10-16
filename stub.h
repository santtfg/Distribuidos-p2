#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/param.h>

#define BACKLOG 1
#define MAX_CLIENTS 100

struct Stub {
    int tcp_socket;
    char          pc[20];
    unsigned int  clock_lamport;

    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;

    pthread_t threads[MAX_CLIENTS]; 
    int clients[MAX_CLIENTS];
    int current_clients;
};


int initServer(char *ip, int port, char *pc, int current_clients);
int initClient(char *ip, int port, char *pc);

int ProcessPetitions(void* thread_server);

void *thread_server(void *args);


int send_message(int operation, char *pc);

int receive_message(int client_socket);


void update_lamport(int tiempo);

int GetClockLamport();

///////
enum operations {
    READY_TO_SHUTDOWN = 0,
    SHUTDOWN_NOW,
    SHUTDOWN_ACK
};

struct message {
    char          origin[20];
    enum operations    action;
    unsigned int  clock_lamport;
};