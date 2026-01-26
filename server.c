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
    addr.sin_port = htons(6379); //port 6379 in byte network order (htons -> converts int host to network 16-bit)
    addr.sin_addr.s_addr = htonl(INADDR_ANY); //any intterface in network order (htonl -> same as htons but long 32-bit)

    //now that we have the server's address we can bind to the socket using bind()
    int rv = bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)); //cast address to (struct sockaddr *) avoid compile warnings & tells bind to treat sockaddr_in as generic type

    //bind socket to address/port
    if ((server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
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

    printf("Server is listening on port 6379...\n");

    //close socket
    close(server_fd);
    return 0;
}