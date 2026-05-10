#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

// void *receive_message(void *args) {
//     if(recv(int fd, void *buf, size_t n, int flags) < 0) {
// 	perror("recv()");
// 	exit(EXIT_FAILURE);
//     }
// }

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

    // Step 4: listen for connections
    printf("listening for client connections...\n");
    if (listen(listening_socket_fd, 5) < 0) {
	perror("listen()");
	exit(EXIT_FAILURE);
    }

    // Step 5: accept the incoming connections that are in the queue
    // NOTE: must accept multiple clients
    // 
    // while (1) {
    // 	accept()
    // 	message client that they have connected to the server using send()
    // 	thread for recv()
    // 	thread for send()
    // }
    struct sockaddr_in connected_socket_address;
    memset(&connected_socket_address, 0, sizeof(connected_socket_address)); // sets all the members of a struct to 0

    socklen_t connected_socket_addr_len = sizeof(connected_socket_address);
    int connected_socket_fd = accept(listening_socket_fd, (struct sockaddr *)&connected_socket_address, &connected_socket_addr_len); // Note: accept() is a blocking call
    if (connected_socket_fd < 0) {
	perror("accept()");
	exit(EXIT_FAILURE);
    }

    // Step 6: send message to client
    char *message = "You have reached the server";
    if (send(connected_socket_fd, message, strlen(message), 0) < 0) {
	perror("accept()");
	exit(EXIT_FAILURE);
    }

	//    // Step 7: read and write loop
	//    while (1) {
	// // prompt the user to send a message
	// pthread_t recv_thread;
	// pthread_create(&recv_thread, NULL, receive_message, NULL);
	// // receive messages sent by the client and print it to stdout
	//    }

    printf("A client has connected!\n");
    return 0;
}
