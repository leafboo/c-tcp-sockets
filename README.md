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
This program is going to be a real-time messaging program. The transmission control protocol I will
be using will be TCP, and uses sockets under the hood!

## Mental model
There will be two programs, `server.c` and `client.c`. 
![server-client diagram](https://www.codequoi.com/images/socket-c/sockets-en.drawio.png)
1. Both the server and the client will create a socket with the `socket()` function. 
2. In the server program, we will use `bind()` to give an address to the socket.
3. `listen()` converts a socket into a listening socket and enables the kernel to make a queue which
stores completed TCP connections.
4. `accept()` is a blocking call. If the queue is not empty, it removes the first completed 
connection from the queue and returns a file descriptor of a new socket created representing that
connection. In that socket, it has both the local (server) IP:port and the remote (client) 
IP:port; this is called the 4 tuple of a socket.
5. 
