#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

// Structure to pass data to the thread
typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
} client_info;

// Mutex for thread-safe operations
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Function to handle client connection
void *handle_client(void *arg) {
    client_info *client = (client_info *)arg;
    char buffer[BUFFER_SIZE];
    int read_size;
    
    // Get client IP address
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(client->client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
    
    // Log client connection
    pthread_mutex_lock(&lock);
    printf("Client connected: %s:%d\n", client_ip, ntohs(client->client_addr.sin_port));
    pthread_mutex_unlock(&lock);
    
    // Receive messages from client
    while ((read_size = recv(client->client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        // Null-terminate the received data
        buffer[read_size] = '\0';
        
        // Log received message
        pthread_mutex_lock(&lock);
        printf("Received from %s:%d: %s", client_ip, ntohs(client->client_addr.sin_port), buffer);
        pthread_mutex_unlock(&lock);
        
        // Echo the message back to client
        send(client->client_socket, buffer, read_size, 0);
    }
    
    // Check for connection closed or error
    if (read_size == 0) {
        pthread_mutex_lock(&lock);
        printf("Client disconnected: %s:%d\n", client_ip, ntohs(client->client_addr.sin_port));
        pthread_mutex_unlock(&lock);
    } else if (read_size == -1) {
        perror("recv failed");
    }
    
    // Close the socket and free the client_info struct
    close(client->client_socket);
    free(client);
    
    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Could not create socket");
        return 1;
    }
    
    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return 1;
    }
    
    // Prepare the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    
    // Listen for incoming connections
    if (listen(server_socket, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        return 1;
    }
    
    printf("Server started on port %d. Waiting for connections...\n", PORT);
    
    // Accept and handle client connections
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len))) {
        // Create client_info struct to pass to the thread
        client_info *client = malloc(sizeof(client_info));
        if (client == NULL) {
            perror("Memory allocation failed");
            close(client_socket);
            continue;
        }
        
        client->client_socket = client_socket;
        client->client_addr = client_addr;
        
        // Create a new thread to handle the client
        if (pthread_create(&thread_id, NULL, handle_client, (void *)client) < 0) {
            perror("Thread creation failed");
            free(client);
            close(client_socket);
            continue;
        }
        
        // Detach the thread to allow it to clean up automatically
        pthread_detach(thread_id);
    }
    
    // Should not reach here unless accept fails
    if (client_socket < 0) {
        perror("Accept failed");
        return 1;
    }
    
    // Close the server socket
    close(server_socket);
    
    return 0;
}