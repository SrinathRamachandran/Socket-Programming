#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;

int main() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    string message;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Define the server address
    address.sin_family = AF_INET;
    address.sin_port = htons(65432);
    inet_pton(AF_INET, "10.0.2.15", &address.sin_addr);

    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    cout << "Server listening on port 65432" << endl;

    // Accept a client connection
    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    cout << "Connection established with client" << endl;

    // Communication loop
    while (true) {
        cout << "Enter a message to send to the client (type 'exit' to end session): ";
        getline(cin, message);

        if (message == "exit") {
            send(client_socket, message.c_str(), message.length(), 0);
            cout << "Ending session." << endl;
            break;
        }

        // Send message to the client
        send(client_socket, message.c_str(), message.length(), 0);
        cout << "Message sent to client" << endl;

        // Receive a response from the client, if any
        int valread = read(client_socket, buffer, 1024);
        if (valread > 0) {
            cout << "Message from client: " << buffer << endl;
        }
        memset(buffer, 0, sizeof(buffer));  // Clear buffer for next message
    }

    // Close the sockets
    close(client_socket);
    close(server_fd);

    return 0;
}
