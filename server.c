#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

int main() {
    
    int server_fd;

    //create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //check if socket is successfull 
    if(server_fd < 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    //print socket on success
    printf("Socket created successfully! fd: %d\n", server_fd);
    
    //store the server's address information. Fields set below
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET; //address family
    addr.sin_port = htons(6380); //port 6379 in byte network order (htons -> converts int host to network 16-bit)
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //any intterface in network order (htonl -> same as htons but long 32-bit)

    //reusable socket
    int val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0) {
        perror("Setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //now that we have the server's address we can bind to the socket using bind()
    //bind socket to address/port
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //start listening
    if(listen(server_fd, 10) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //print success
    printf("Server is listening on port 6380...\n");

    //struct for client address
    struct sockaddr_in client_addr = {0};
    socklen_t client_len = sizeof(client_addr); //length of client address

    //infinet loop to keep accepting clients
    while(1) {

        //init client socket
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        
        int flags = fcntl(client_fd, F_GETFL, 0);
        fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

        //try and accept client
        if(client_fd < 0) {
            perror("Accept failed");
            continue; //keep trying for next client
        }

        printf("Connection accepted from a clinet!\n");
        
        //nested loop to keep readingn from client
        while(1) {

            //print when successfully accepted
            
            char buffer[1024]; //define a buffer with limit of 1024 bytes
            ssize_t bytes_received = read(client_fd, buffer, sizeof(buffer) - 1); //read into buffer
            
            //error or client disconnected
            if(bytes_received <= 0) {
                break;
            }
            
            buffer[bytes_received] = '\0'; //null terminator to determin end of string
            printf("Client sent: %s\n", buffer); //print client input
            
            //clasic redis PING PONG to test if connection works
            if(strncmp(buffer, "PING", 4) == 0) {
                write(client_fd, "+PONG\r\n", 7); //respond with PONG
            }

            if(strcmp(buffer, "q\n") == 0) {
                break;
            }
        }
        
        printf("Client disconnected, waiting for new connection...");
        close(client_fd); //close this spesific connection
    }

    close(server_fd); //close socket
    return 0;
}