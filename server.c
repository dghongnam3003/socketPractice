#include <getopt.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define INFINITE 100000

int main(int argc, char *argv[]) {
    int opt;
    char *root = NULL;
    int port = 0;
    char *ip;

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
                ip = optarg;
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
    

    int server_fd, new_socket, valread, client_socket[INFINITE], max_clients = INFINITE, activity, sd;
    int max_sd;
    struct sockaddr_in address;
    int option = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = { 0 };

    //set socket descriptors
    fd_set readfds;


    char* hello = "Chuan bi di ve thoi";

    //initialise all client_socket[] to 0 so not checked 
    for (int i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }


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

    puts("Waiting for connection...");

    while(TRUE) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add server_fd to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        //add child socket to set
        for (int i = 0; i < max_clients; i++) {
            //socket descriptor
            sd = client_socket[i];

            //add to the list if valid
            if (sd > 0) FD_SET(sd, &readfds);

            if (sd > max_sd) max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL , 
        //so wait indefinitely
        activity = select( max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }

        //If something happened on the server_fd socket , 
        //then its an incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, 
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }

            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            printf("[*] Start file server successfully in 0.0.0.0:%s\n", ip);


            //take client ip
            char client_ip[INET_ADDRSTRLEN];
            struct sockaddr_in* addr_in = (struct sockaddr_in*)&address;
            inet_ntop(AF_INET, &(addr_in->sin_addr), client_ip, INET_ADDRSTRLEN);
            printf("Client IP address: %s\n", client_ip);


            //send message Hello from server
            valread = read(new_socket, buffer, 1024);
            printf("%s\n", buffer);
            send(new_socket, hello, strlen(hello), 0);
            printf("Hello message sent\n");

            //add new socket to array of sockets 
            for (int i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  
                         
                    break;  
                }  
            }
        }

        //else its some IO operation on some other socket
        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if(FD_ISSET(sd, &readfds)) {
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , buffer, 1024)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                          inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                         
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                     
                //Echo back the message that came in 
                else 
                {  
                    //set the string terminating NULL byte on the end 
                    //of the data read 
                    valread = read(sd, buffer, 1024);
                    printf("%s\n", buffer);
                    send(sd, hello, strlen(hello), 0);
                    printf("Hello message sent\n"); 
                }
            }
        }
    }

    /*if ((new_socket
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
    shutdown(server_fd, SHUT_RDWR);*/
    

    return 0;
}