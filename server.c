#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {

    // Step 1: declare the socket
    int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_fd < 0) {
	perror("socket()");
	exit(EXIT_FAILURE);
    }
    // Step 2: create a struct with the address details for the socket
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address)); // sets all the members of a struct in C to 0
    server_address.sin_family = AF_INET; // sets to IPv4
    server_address.sin_port = htons(5100); // sets the port to 5100
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // sets IP address to 0.0.0.0
    
    // Step 3: assigns the address in the struct to the socket via socket_fd returned by socket()
    if (bind(server_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
	perror("bind()");
	exit(EXIT_FAILURE);
    }

    // Step 4: listen for connections
    printf("listening for client connections...\n");
    if (listen(server_socket_fd, 5) < 0) {
	perror("listen()");
	exit(EXIT_FAILURE);
    }

    // Step 5: accept the incoming connections that are in the queue
    struct sockaddr_in client_address;
    memset(&client_address, 0, sizeof(client_address)); // sets all the members of a struct to 0

    socklen_t client_addr_len = sizeof(client_address);
    int client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_address, &client_addr_len); // Note: accept() is a blocking call
    if (client_socket_fd < 0) {
	perror("accept()");
	exit(EXIT_FAILURE);
    }

    // Step 6: send message to client
    char *message = "You have reached the server";
    if (send(client_socket_fd, message, strlen(message), 0) < 0) {
	perror("accept()");
	exit(EXIT_FAILURE);
    }

    printf("A client has connected!");
    return 0;
}
