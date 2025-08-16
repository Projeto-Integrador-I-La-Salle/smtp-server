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
#include <format>
#include <string>
#include <vector>

std::vector<std::string> split(char *string, char *delimiter) {
    std::vector<std::string> result;

    char *t = strtok(string, delimiter);
    while ( t != nullptr ) {
        result.push_back( t );
        t = strtok( nullptr, delimiter );
    }

    return result;
}

int main() {
    ssize_t bytes_read;
    // socket create and verification.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if ( serverSocket == -1 ) {
      std::cout << "[ERROR]: Socket creation failed..." << std::endl;
      exit(0);
    }
    std::cout << "Socket successfully created." << std::endl;

    // assign IP and PORT.
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080); // TODO: listen on port 25.
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if ( bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) != 0 ) {
        std::cout << "[ERROR]: Socket bind failed." << std::endl;
        exit(0);
    }
    std::cout << "Socket successfully binded." << std::endl;

    if ( listen(serverSocket, 5) != 0 ) {
        std::cout << "[ERROR]: Listen failed." << std::endl;
        exit(0);
    }
    std::cout << "Server listening on port 8080" << std::endl;

    int clientSocket = accept( serverSocket, nullptr, nullptr );
    if ( clientSocket < 0 ) {
      std::cout << "[ERROR]: Server accept failed." << std::endl;
      exit(0);
    }
    std::cout << "Connection accepted." << std::endl;

    const char *res = "220 smtp.example.com\n";
    write( clientSocket, res, strlen( res ) );

    // main loop: wait for multiple messages.
    while ( true ) {
        char buffer[1024] = { 0 };
        bytes_read = read( clientSocket, buffer, sizeof(buffer) -1 );
        if ( bytes_read < 0 ) {
            std::cout << "Read failed";
            break;
        } else if ( bytes_read == 0 ) {
            std::cout << "Client disconnected.";
            break;
        }

        std::string str = buffer;
        std::string client_identity = str.substr(4);
        if ( str.substr( 0,4 ) != "HELO" ) {
          std::cout << "[ERROR]: Invalid client greeting. Expected HELO.";
          break;
        }

        std::cout << "MAIL FROM PHASE" << std::endl;
        // respond to the client
        std::string res = std::string("250 ") + client_identity + "\n";
        write( clientSocket, res.c_str(), res.size() );

        // MAIL FROM:<example@gmail.com>
        std::string mail_from = "";

        bytes_read = read( clientSocket, buffer, sizeof(buffer) -1 );
        if ( bytes_read < 0 ) {
            std::cout << "Read failed";
            break;
        } else if ( bytes_read == 0 ) {
            std::cout << "Client disconnected.";
            break;
        }

        mail_from = buffer;
        if ( mail_from.substr( 0, 4 ) != "MAIL" ) {
            std::cout << "Invalid message.";
            break;
        }
        // todo: validate the rest of the request.

        res = "250 Ok\n";
        write( clientSocket, res.c_str(), res.size() );


        // RCPT TO:<example@gmail.com>
        std::vector<std::string> rcpt_vector;
        while( true ) {
            std::string rcpt_to = "";

            bytes_read = read( clientSocket, buffer, sizeof(buffer) -1 );
            if ( bytes_read < 0 ) {
                std::cout << "Read failed";
                break;
            } else if ( bytes_read == 0 ) {
                std::cout << "Client disconnected.";
                break;
            }

            rcpt_to = buffer;

            char *delimiter = " ";
            if ( split( buffer, delimiter )[0] != "RCPT" &&
                 split( buffer, delimiter )[1] != "TO" ) {
                std::cout << "[ERROR]: Invalid parameters.";
                res = "501 Syntax error in parameters or arguments";
                write( clientSocket, res.c_str(), res.size() );
                break;
            }

            // TODO: validate other parts of the string.

            rcpt_vector.push_back( rcpt_to );

            // finish rcpt and enters on mail content
            if ( sizeof( rcpt_vector ) != 0 && rcpt_to.substr( 0, 4 )  == "DATA" ) {
                break;
            }
        }

        // DATA
    }

    close( clientSocket );
    std::cout << "Connection closed." << std::endl;

    close( serverSocket );

    return 0;
}

