#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    // Define the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(65432);
    
    // Convert IP address of the server to binary and store it
    if (inet_pton(AF_INET, "192.168.56.101", &serv_addr.sin_addr) <= 0) { // Replace with VM1’s IP address
        std::cerr << "Invalid address or Address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }
    std::cout << "Connected to server" << std::endl;

    // Receive message from the server
    int valread = read(sock, buffer, 1024);
    std::cout << "Message from server: " << buffer << std::endl;

    // Close the socket
    close(sock);

    return 0;
}
