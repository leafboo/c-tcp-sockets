# Sockets in C
This is my first blog I've written. My goal in writing this and future blogs is to be able to dive 
deep into subjects and to store it digitally so I can look at it whenever I need a refresher on the subject.


## What are Sockets???
A socket is an endpoint that allows the communication between 2 programs(client and server). 
The TL;DR of a socket is that it's basically the IP address + the port of a computer.
<details>
    <summary>Detailed info on sockets</summary>
    Deep dive info on sockets
</details>


## The program
The program will be a chat room using TCP sockets. There will be client.c and server.c where the 
server's job is to get the message from one client and send it to all other clients that is 
connected to the server.

## Understanding the TCP Client-Server Flow
![server-client diagram](/images/c-tcp-sockets-diagram.png)
> Image credit: [codequoi.com](https://www.codequoi.com/en/sockets-and-network-programming-in-c/)
1. Both the server and the client will create a socket with the `socket()` function. 
2. In the server program, we will use `bind()` to give an address to the socket.
3. `listen()` converts a socket into a listening socket and enables the kernel to make a queue which
stores completed TCP connections.
4. `accept()` is a blocking call. If the queue is not empty, it removes the first completed 
connection from the queue and returns a file descriptor of a new socket created representing that
connection. In that socket, it has both the local (server) IP:port and the remote (client) 
IP:port; this is called the 4 tuple of a socket.
5. The `connect()` function establishes a connection to a listening socket in a server. 

## One server multiple clients 
![multiple client diagram](/images/one-server-multiple-clients.png) <br> <br>
When the server executes the `listen()` function, it does two things: creates a listening socket and 
it also tells the kernel to make a queue that stores TCP connections of clients. The size of the queue
is the backlog(2nd) argument of the `listen()`function (note: the size doesn't refer to how many clients can 
connect to the server). <br> <br>

When the server executes the `accept()` function, the TCP connection in the queue gets 
released and a connection socket unique to that client is created in the server, making it possible for 
the client and server to communicate. 

