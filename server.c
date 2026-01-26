#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

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

        //try and accept client
        if(client_fd < 0) {
            perror("Accept failed");
            continue; //keep trying for next client
        }

        //print when successfully accepted
        printf("Connection accepted from a clinet!\n");

        close(client_fd); //close this spesific connection
    }

    close(server_fd); //close socket
    return 0;
}