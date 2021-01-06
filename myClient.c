// File Name:myClient.c
//
//Module:CE4206
//Assignment: #5
//Author: Eoghan O'Connor
//
// File Description: This is the myClient.c file.
//                   The client gets the address to the socket
//                   The client connects to the socket
//                   The the client prompts user for input
//                   The client sends input to server.
//                   If the word say "End" the connection with client is closed
//                    and the client program closes down.
//////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

// MAIN
int main(int argc, char *argv[])
{

    struct sockaddr_in servAddr; // declare socket address structure
    int clientFd;                // socket descriptor, behaves like file descriptor
    char receiveBuff[1024] = {0};
    char transmitBuff[1024] = {0};

    // Check command line argument
    if (argc != 2)
    {
        printf("\n\n Use single argument: IP address of server\n");
        exit(EXIT_FAILURE);
    }
    // Create the socket
    clientFd = socket(PF_INET, SOCK_STREAM, 0); // Note PF_, not AF_

    if (clientFd == -1)
    {
        perror("did not create the socket");
        exit(EXIT_FAILURE);
    }

    memset(&servAddr, 0, sizeof(servAddr)); // clear the socket address structure

    // Fill in the address information for the socket that we want to contact
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000);

    if (inet_pton(AF_INET, argv[1], &servAddr.sin_addr) <= 0)
    {
        perror("\n inet_pton error\n");
        close(clientFd);
        exit(EXIT_FAILURE);
    }

    // Connect to remote host by attaching specified server address
    if (connect(clientFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        perror("\n connect error\n");
        close(clientFd);
        exit(EXIT_FAILURE);
    }

    // Read and print server connection word
    read(clientFd, receiveBuff, sizeof(receiveBuff));
    printf("%s\n", receiveBuff);
    memset(receiveBuff, 0, sizeof(receiveBuff)); // Clear the receive buffer

    while (1)
    {
        //Message for user
        printf("Enter \033[1;32m End \033[0m to end client program\nEnter word for Server:\n");
        scanf("%s", transmitBuff);
        // If the received string is "End" than close the connection for this client

        // Send message to client
        if ((write(clientFd, transmitBuff, strlen(transmitBuff))) == -1)
        {
            perror("write to connection failure");
            close(clientFd);
            exit(EXIT_FAILURE);
        }
        // If the received string is "End" than close the connection for this client
        if (strcmp(transmitBuff, "End") == 0)
        {
            printf("Closing down this client connection\n");
            close(clientFd);    // Close the connection
            exit(EXIT_SUCCESS); // break out of the for() loop and service the next client
        }
        read(clientFd, receiveBuff, sizeof(receiveBuff)); // read
        printf("%s\n", receiveBuff);                      // print
    }
}