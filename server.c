#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

typedef struct {
    int socket_fd;
    struct sockaddr_in connected_socket_address;
    socklen_t connected_socket_addr_len;
} Client;

void *recv_function(void *client);

int main(int argc, char *argv[]) {

    // Step 1: declare the socket
    int listening_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket_fd < 0) {
	perror("socket()");
	exit(EXIT_FAILURE);
    }
    // Step 2: create a struct with the address details for the socket
    struct sockaddr_in listening_socket_address;
    memset(&listening_socket_address, 0, sizeof(listening_socket_address)); // sets all the members of a struct in C to 0
    listening_socket_address.sin_family = AF_INET; // sets to IPv4
    listening_socket_address.sin_port = htons(5100); // sets the port to 5100
    listening_socket_address.sin_addr.s_addr = htonl(INADDR_ANY); // sets IP address to 0.0.0.0
    
    // Step 3: assigns the address in the struct to the socket via socket_fd returned by socket()
    if (bind(listening_socket_fd, (struct sockaddr *)&listening_socket_address, sizeof(listening_socket_address)) < 0) {
	perror("bind()");
	exit(EXIT_FAILURE);
    }

    // Step 4: transform the active socket to a passive (listening) socket
    printf("listening for client connections...\n");
    if (listen(listening_socket_fd, 5) < 0) {
	perror("listen()");
	exit(EXIT_FAILURE);
    }

    // Step 5: accept the incoming connections that are in the queue
    // while (1) {
    // 	accept 
    // 	make a thread for the recv()
    //	in the thread:
    //		if a message is received from a client, send() the message to other clients
    // }
    // in the custom client struct, I need the members:
    // int socket_fd;
    // struct sockaddr_in connected_socket_address;
    // socklen_t connected_socket_addr_len
    //
    Client clients[5]; // we can have 5 connected clients socket
    int counter = 0;

    while (counter < 5) {
    
	memset(&clients[counter].connected_socket_address, 0, sizeof(clients[counter].connected_socket_address)); // sets all the members of a struct to 0
	// TODO: simplify this code later
	socklen_t connected_socket_addr_len = sizeof(clients[counter].connected_socket_address);
	clients[counter].socket_fd = accept(listening_socket_fd, (struct sockaddr *)&(clients[counter].connected_socket_address), &connected_socket_addr_len); // Note: accept() is a blocking call
																	    //
	if (clients[counter].socket_fd < 0) {
	    perror("accept()");
	    exit(EXIT_FAILURE);
	}

	// Step 6: send message to client
	char *message = "You have reached the server";
	if (send(clients[counter].socket_fd, message, strlen(message), 0) < 0) {
	    perror("accept()");
	    exit(EXIT_FAILURE);
	}

	// Step 7: make a thread for receiving messages from the client
	pthread_t client1_thread;
	pthread_create(&client1_thread, NULL, recv_function, &(clients[counter])); // TODO: Pass the connected_socket_fd to the recv_function here

	printf("A client has connected!\n");
    }

    return 0;
}

void *recv_function(void *client) {

    Client *c = client; 

    while (1) {
	char *msg_frm_client = malloc(1024);

	if (recv(c->socket_fd, msg_frm_client, 1024, 0) < 0) {
	    free(msg_frm_client);
	    perror("recv()");
	    exit(EXIT_FAILURE);
	}
	// send message to other clients using their sock_fd
	// PROBLEM: this doesn't know the socket_fd of the clients that came after it
	printf("Message from the client: %s\n", msg_frm_client);
	free(msg_frm_client);
    }

    return NULL;
}
