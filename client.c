#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>

/* macros */
#define PORT 5100
#define BUFFER_SIZE 1024

void *receive_messages(void *p_client_socket_fd);
void *safe_malloc(size_t size);

int
main(int argc, char *argv[]) {
	// TODO: clients should be able to set their username
	if (argc < 2) {
		printf("Please input your username\n");
		exit(EXIT_FAILURE);
	} else if (argc > 2) {
		printf("Number of arguments is too much\n");
		exit(EXIT_FAILURE);
	}

	// Step 1: declare the socket
	int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	// Step 2: define the address of the server socket we want to connect to
	struct sockaddr_in server_address;
	memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET; // sets to IPv4
	server_address.sin_port = htons(PORT); // sets the port to 5100
	if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) != 1) {
		perror("inet_pton()");
		exit(EXIT_FAILURE);
	}

	// Step 3: connect to the socket
	if (connect(client_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
		perror("connect()");
		exit(EXIT_FAILURE);
	}

	// Send the username to the server
	if (send(client_socket_fd, argv[1], strlen(argv[1]), 0) < 0) {
		perror("send()");
		exit(EXIT_FAILURE);
	}

	// Step 4: receive the welcome message from the server
	char *msg_frm_server = safe_malloc(BUFFER_SIZE);
	if (recv(client_socket_fd, msg_frm_server, BUFFER_SIZE, 0) < 0) {
		perror("recv()");
		exit(EXIT_FAILURE);
	}
	printf("message from server: %s\n", msg_frm_server);

	// Create a thread for listening for messages from other clients
	pthread_t thread_id;
	pthread_create(&thread_id, NULL, receive_messages, &client_socket_fd);

	// Step 5: make a loop for sending messages to the server
	while (1) {
		char *message = NULL;
		size_t bufsize = 0;

		// get the characters in stdin and put it in a buffer
		if (getline(&message, &bufsize, stdin) < 0) { // NOTE: this also stores the newline \n in the buffer
			if (feof(stdin)) {
				exit(EXIT_SUCCESS);
				free(message);
			}
			perror("getline()");
			exit(EXIT_FAILURE);
			free(message);
		}
		if (send(client_socket_fd, message, strlen(message) - 1, 0) < 0) {
			free(message);
			perror("send()");
			exit(EXIT_FAILURE);
		}
		free(message);
	}
	return 0;
}

void *
receive_messages(void *p_client_socket_fd) {
	int client_socket_fd = *(int *)p_client_socket_fd;
	while (1) {
		char *msg_frm_server = safe_malloc(BUFFER_SIZE); // NOTE: I know this could overflow, but oh well I'll deal with it later
		ssize_t bytes_received = recv(client_socket_fd, msg_frm_server, BUFFER_SIZE, 0);

		if(bytes_received < 0) {
			perror("recv()");
			exit(EXIT_FAILURE);
		} else if(bytes_received == 0) {
			free(msg_frm_server);
			close(client_socket_fd); // NOTE: this is not required bc the OS closes the fd when the program closes but it's considered good practice
			exit(EXIT_FAILURE);
		}

		printf("%s\n", msg_frm_server);
		free(msg_frm_server);
	}
	return NULL;
}

void *
safe_malloc(size_t size) {
	void *ptr = malloc(size);
	if (ptr == NULL) {
		fprintf(stderr, "malloc() failed. Insufficient memory.");
		exit(EXIT_FAILURE);
	}
	return ptr;
}
