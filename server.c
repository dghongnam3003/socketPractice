#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int opt;
    char *root = NULL;
    int port = 0;

    const struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"root", required_argument, NULL, 'r'},
        {"port", required_argument, NULL, 'p'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "hr:p:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                printf("Help message\n");
                exit(EXIT_SUCCESS);
            case 'r':
                root = optarg;
                break;
            case 'p':
                port = atoi(optarg);
                printf("[*] Start file server successfully in 0.0.0.0:%s\n", optarg);
                break;
            case '?':
                printf("Unknown option: %s\n", argv[optind-1]);
                break;
            case ':':
                printf("Option requires an argument: %s\n", argv[optind-1]);
                break;
            default:
                exit(EXIT_FAILURE);
        }
    }
    

    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int option = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };
    char* hello = "Hello from server";


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
  
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET,
                   SO_REUSEADDR | SO_REUSEPORT, &option,
                   sizeof(option))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
  
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr*)&address,
             sizeof(address))
        < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket
         = accept(server_fd, (struct sockaddr*)&address,
                  (socklen_t*)&addrlen))
        < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    char client_ip[INET_ADDRSTRLEN];
    struct sockaddr_in* addr_in = (struct sockaddr_in*)&address;
    inet_ntop(AF_INET, &(addr_in->sin_addr), client_ip, INET_ADDRSTRLEN);
    printf("Client IP address: %s\n", client_ip);

    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);
    send(new_socket, hello, strlen(hello), 0);
    printf("Hello message sent\n");
  
    // closing the connected socket
    close(new_socket);
    // closing the listening socket
    shutdown(server_fd, SHUT_RDWR);
    

    return 0;
}