#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <mutex>
using namespace std;

vector<thread> client_threads;
mutex mtx; // To synchronize output

void handle_client(int client_socket, int client_id) {
    char buffer[1024] = {0};
    string message;

    // Communication loop with this client
    while (true) {
        // Receive a message from the client
        int valread = read(client_socket, buffer, 1024);
        if (valread <= 0) {
            mtx.lock();
            cout << "Client " << client_id << " disconnected." << endl;
            mtx.unlock();
            break;
        }

        // Display the message from the client
        mtx.lock();
        cout << "Message from client " << client_id << ": " << buffer << endl;
        mtx.unlock();
       
        memset(buffer, 0, sizeof(buffer));  // Clear buffer for the next message

        // Send an automated response to the client
        message = "Hello from server to client " + to_string(client_id);
        send(client_socket, message.c_str(), message.length(), 0);
    }

    // Close client socket when done
    close(client_socket);
}

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int client_id = 0;  // Used to label each client

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    inet_pton(AF_INET, "10.0.2.15", &address.sin_addr); //drop in your server address here
    address.sin_port = htons(65432);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 5) < 0) {  // Accepts up to 5 clients waiting in the queue
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    cout << "Server listening on port 65432" << endl;

    // Accept multiple clients in a loop
    while (true) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue; // Skip to the next iteration
        }
       
        client_id++;  // Increment client ID for each new connection

        mtx.lock();
        cout << "New client connected with ID: " << client_id << endl;
        mtx.unlock();

        // Create a new thread to handle the client
        client_threads.push_back(thread(handle_client, client_socket, client_id));
    }

    // Join all client threads before exiting the server
    for (thread &t : client_threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Close the server socket
    close(server_fd);

    return 0;
}

