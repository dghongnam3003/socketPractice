#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void send_message(int client_socket, const char* message) {
    // Implementation of send_message function is not shown
    // Assume it sends the message to the client socket
}

void put_file(int client_socket, const char* directory, const char* filename, const char* content) {
    char path[256];
    snprintf(path, sizeof(path), "%s/%s", directory, filename);
    
    // Open the file in binary write mode
    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        // If file opening failed, send an error message to the client
        send_message(client_socket, "Cannot create file.");
        printf("Cannot create file.\n");
        return;
    }
    
    // Write content to the file
    size_t content_length = strlen(content);
    fwrite(content, sizeof(char), content_length, file);
    fclose(file);
    
    // Send a success message to the client
    send_message(client_socket, "File created/updated successfully.");
    printf("Created\n");
}

int main() {
    const char* directory = "/home/yong/new";
    const char* filename = "example2.txt";
    const char* content = "Hello tml2";

    put_file(0, directory, filename, content);

    return 0;
}

