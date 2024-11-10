#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Socket creation error" << endl;
        return -1;
    }

    // Define the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(65432);
   
    // Convert IP address of the server to binary and store it
    if (inet_pton(AF_INET, "10.0.2.15", &serv_addr.sin_addr) <= 0) { // Replace with server IP address
        cerr << "Invalid address or Address not supported" << endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cerr << "Connection Failed" << endl;
        return -1;
    }
    cout << "Connected to server" << endl;

    // Communication loop
    while (true) {
        int valread = read(sock, buffer, 1024);
        if (valread > 0) {
            string received_message(buffer);
            cout << "Message from server: " << received_message << endl;

            if (received_message == "exit") {
                cout << "Server ended the session." << endl;
                break;
            }

            // Optionally, send a response to the server
            string client_response;
            cout << "Enter your response to the server: ";
            getline(cin, client_response);
            send(sock, client_response.c_str(), client_response.length(), 0);
        }
        memset(buffer, 0, sizeof(buffer));  // Clear buffer for the next message
    }

    // Close the socket
    close(sock);

    return 0;
}