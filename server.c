#include <asm-generic/socket.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* macros */
#define BUFFER_SIZE 1024
#define PORT 5100

typedef struct {
    int *p_connected_socket_fds;
    size_t arr_bytes; 
} SharedArrayInfo;

typedef struct {
    char *p_username;
    int sender_socket_fd;
    SharedArrayInfo *p_shared_array_info;
} ClientHandlerArgs;

void *recv_function(void *client);
void broadcast_new_client(int *connected_sockets, int connected_sockets_len, int sender_socket_fd, char *username);

int main(int argc, char *argv[]) {
    // TODO: technically not required but it's good practice to close() the socket file descriptors
    // of connected sockets

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
    listening_socket_address.sin_port = htons(PORT); // sets the port to 5100
    listening_socket_address.sin_addr.s_addr = htonl(INADDR_ANY); // INADDR_ANY (0.0.0.0) allows the socket to accept connections on any local network interface when used with bind()
								  
    // Credits: Beej's Guide to Network Programming
    // For removing the "bind(): Address already in use" error
    int yes = 1;
    //char yes='1'; // Solaris people use this

    // lose the pesky "Address already in use" error messages
    setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    
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
    int connected_socket_fds[5];
    char *username_pointers[5];
    SharedArrayInfo shared_array_info = { connected_socket_fds, sizeof(connected_socket_fds) }; 
    ClientHandlerArgs client_handler_args[5];
    memset(&connected_socket_fds, 0, sizeof(connected_socket_fds));
    int counter = 0;

    while (counter < 5) {
	ClientHandlerArgs *p_client_handler_args = &client_handler_args[counter];
	int sender_socket_fd = accept(listening_socket_fd, 0, 0);

	if (sender_socket_fd < 0) {
	    perror("accept()");
	    exit(EXIT_FAILURE);
	}

	// Get the username of the client
	char *username = malloc(BUFFER_SIZE);
	if (recv(sender_socket_fd, username, BUFFER_SIZE, 0) < 0) {
	    free(username);
	    perror("recv()");
	    exit(EXIT_FAILURE);
	}
	username_pointers[counter] = username;
	username = NULL;

	p_client_handler_args->p_username = username_pointers[counter];
	p_client_handler_args->sender_socket_fd = sender_socket_fd;
	p_client_handler_args->p_shared_array_info = &shared_array_info;

	connected_socket_fds[counter] = sender_socket_fd;


	// Step 6: send message to client
	char message[BUFFER_SIZE];
	sprintf(message, "You have reached the chat server. Welcome %s", p_client_handler_args->p_username); // TODO: check later if this needs to be error handled

	if (send(sender_socket_fd, message, strlen(message), 0) < 0) {
	    perror("send()");
	    exit(EXIT_FAILURE);
	}

	printf("A client has connected: %s\n", p_client_handler_args->p_username);

	// TODO: inform other clients connected to the server that another client has connected
	int arr_len = p_client_handler_args->p_shared_array_info->arr_bytes / sizeof(int);
	broadcast_new_client(connected_socket_fds, arr_len, sender_socket_fd, username_pointers[counter]);

	// Step 7: make a thread for receiving messages from the client and sending the received message to other clients in the array
	// TODO: make an array (maybe) for storing the thread IDs
	pthread_t thread_id; // this variable stores the ID of the newly created thread
	pthread_create(&thread_id, NULL, recv_function, p_client_handler_args); // TODO: Pass the connected_socket_fd to the recv_function here


	counter++;
    }
    return 0;
}

char *format_message(char *username, char *message) {
    char *result = malloc(strlen(username) + strlen(message) + 1);
    sprintf(result, "%s: %s", username, message);
    return result;
}

void broadcast_new_client(int *connected_socket_fds, int connected_sockets_len, int sender_socket_fd, char *username) {
    char *message = malloc(strlen(username) + 100);
    sprintf(message, "%s has connected!\n", username);

    for (int i = 0; i < connected_sockets_len; i++) {
	if (sender_socket_fd == connected_socket_fds[i] || connected_socket_fds[i] == 0) {
	    continue;
	}
	if (send(connected_socket_fds[i], message, strlen(message), 0) < 0) {
	    perror("send()");
	    exit(EXIT_FAILURE);
	}
    }
    free(message);
}
  
void *recv_function(void *client_handler_args) {
    ClientHandlerArgs *client = client_handler_args;

    while (1) {
	char *msg_frm_client = malloc(BUFFER_SIZE);

	if (recv(client->sender_socket_fd, msg_frm_client, BUFFER_SIZE, 0) < 0) {
	    free(msg_frm_client);
	    perror("recv()");
	    exit(EXIT_FAILURE);
	}

	char *name_w_message = format_message(client->p_username, msg_frm_client);

	int len = client->p_shared_array_info->arr_bytes / sizeof(int);

	for (int i = 0; i < len; i++) {
	    int sender_socket_fd = client->sender_socket_fd;
	    int recipient_socket_fd = client->p_shared_array_info->p_connected_socket_fds[i]; 

	    if (sender_socket_fd == recipient_socket_fd || recipient_socket_fd == 0) {
		continue;
	    }

	    if(send(recipient_socket_fd, name_w_message, strlen(name_w_message), 0) < 0) {
		free(name_w_message);
		free(msg_frm_client);
		perror("accept()");
		exit(EXIT_FAILURE);
	    }
	}
	free(name_w_message);
	free(msg_frm_client);
    }
    return NULL;
}
