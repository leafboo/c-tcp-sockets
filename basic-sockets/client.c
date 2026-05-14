#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <malloc.h>

int main(int argc, char *argv[]) {

    // Step 1: declare the socket
    int client_socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    // Step 2: define the address of the server socket we want to connect to 
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET; // sets to IPv4
    server_address.sin_port = htons(5100); // sets the port to 5100
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) != 1) {
	perror("inet_pton()");
	exit(EXIT_FAILURE);
    }

    // Step 3: connect to the socket
    if (connect(client_socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
	perror("connect()");
	exit(EXIT_FAILURE);
    }

    // Step 4: read and write 
    char *msg_frm_server = malloc(1024);
    if (recv(client_socket_fd, msg_frm_server, malloc_usable_size(msg_frm_server), 0) < 0) {
	perror("recv()");
	exit(EXIT_FAILURE);
    }

    printf("message from server: %s\n", msg_frm_server);


    return 0;
}
