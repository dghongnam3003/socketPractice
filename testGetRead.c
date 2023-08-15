#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

void send_message(int client_socket, const char* message) {
    printf("Sending message to client:\n%s\n", message);
    // Send the message to the client socket
    // Implementation specific
}

void get_file(int client_socket, const char* directory, const char* filename) {
    char path[1024];
    snprintf(path, sizeof(path), "%s/%s", directory, filename);

    // Open the file in read-only mode
    int file_descriptor = open(path, O_RDONLY);
    if (file_descriptor == -1) {
        send_message(client_socket, "Cannot open file.");
        return;
    }
    
    // Get file size
    off_t file_size = lseek(file_descriptor, 0, SEEK_END);
    lseek(file_descriptor, 0, SEEK_SET);
    
    // Read file content
    char* buffer = (char*)malloc(file_size);
    ssize_t bytes_read = read(file_descriptor, buffer, file_size);
    close(file_descriptor);
    
    // Check if the read operation was successful
    if (bytes_read != file_size) {
        free(buffer);
        send_message(client_socket, "Error reading file.");
        return;
    }
    
    // Send file content to client
    send_message(client_socket, buffer);
    free(buffer);
}

int main() {
    const char* directory = "/home/yong/socket";
    const char* filename = "client.c";
    
    // Example usage of get_file function
    get_file(0, directory, filename); // Assuming client socket value 0 represents standard output
    
    return 0;
}
