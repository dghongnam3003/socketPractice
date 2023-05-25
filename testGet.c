#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void send_message(int client_socket, const char* message) {
    printf("Sending message to client:\n%s\n", message);
    // Send the message to the client socket
    // Implementation specific
}

void get_file(int client_socket, const char* directory, const char* filename) {
    // Construct the full path of the file on the server
    char server_path[512];
    snprintf(server_path, sizeof(server_path), "%s/%s", directory, filename);

    FILE* server_file = fopen(server_path, "rb");
    if (server_file == NULL) {
        send_message(client_socket, "Cannot open file on server.");
        return;
    }

    // Open the file in the client's current directory for writing
    char client_path[512];
    snprintf(client_path, sizeof(client_path), "%s/%s", ".", filename);
    FILE* client_file = fopen(client_path, "wb");
    if (client_file == NULL) {
        fclose(server_file);
        send_message(client_socket, "Cannot create file on client.");
        return;
    }

    // Read chunks of data from the server and write to the client file
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), server_file)) > 0) {
        size_t bytes_written = fwrite(buffer, 1, bytes_read, client_file);
        if (bytes_written != bytes_read) {
            fclose(server_file);
            fclose(client_file);
            send_message(client_socket, "Error writing to client file.");
            return;
        }
    }

    fclose(server_file);
    fclose(client_file);
    send_message(client_socket, "File downloaded successfully.");
}

int main() {
    const char* directory = "/home/yong/new";
    const char* filename = "example.txt";

    // Example usage of get_file function
    get_file(0, directory, filename); // Assuming client socket value 0 represents standard output

    return 0;
}

