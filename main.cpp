/*******************************************************************************
 * @file:       main.cpp
 * @author:     Nathan Berger
 * @date:       2025-08-16
 * @version     1.0
 * @brief       Simple TCP server using sockets.
 *
 * @details     Based on https://www.geeksforgeeks.org/c/tcp-server-client-implementation-in-c/
 ******************************************************************************
 * MIT LICENSE
 *
 * Copyright (c) 2025 Project Name. All rights reserved.
 * Unauthorized copying or use of this file is prohibited.
*******************************************************************************/

#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main() {
    // socket create and verification.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if ( serverSocket == -1 ) {
      cout << "[ERROR]: Socket creation failed..." << endl;
      exit(0);
    }
    cout << "Socket successfully created." << endl;

    // assign IP and PORT.
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // TODO: listen on port 25.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if ( bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0 ) {
        cout << "[ERROR]: Socket bind failed." << endl;
        exit(0);
    }
    cout << "Socket successfully binded." << endl;

    if ( listen(serverSocket, 5) != 0) {
        cout << "[ERROR]: Listen failed." << endl;
        exit(0);
    }
    cout << "Server listening on port 8080" << endl;

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if ( clientSocket < 0 ) {
        cout << "[ERROR]: Server accept failed." << endl;
        exit(0);
    }

    char buffer[1024] = { 0 };
    recv(clientSocket, buffer, sizeof(buffer), 0);
    cout << "Message from client: " << buffer << endl;

    close(serverSocket);

    return 0;
}

