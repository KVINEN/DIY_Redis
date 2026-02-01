#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>

//here we should implement how we handle different commands and how to respond
int handle_command(const char *input, size_t input_len, char *output_buffer, size_t max_output_len);