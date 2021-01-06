// File Name:myServer2.c
//
//Module:CE4206
//Assignment: #5
//Student Name: Eoghan O'Connor
//Student I.D: 16110625
//
// File Description: This is the myServer.c file.
//                   The server creates a socket
//                   The server reads the word from the Socket connection.
//                   The server sends back the fd connection to client.
//                   The server then sends the word back to the client.
//                   The server sends back a time stamp of when the word was recieved
//                   If the word say "End" the connection with client is closed
//
//////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <time.h>
int client = 1, counter = 1;                        // Global counters
struct sockaddr_in servAddr;                        // Declare socket address structure
int sockFd, connFd, rc;                             // Socket descriptors, behave like file descriptors
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER; // Mutex for locking server for threads using it.
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER; // Mutex for locking server for threads using it.
time_t t;                                           // long int for seconds since UNIX epoch

void *ServerFunc();
// MAIN
int main()
{
    memset(&servAddr, 0, sizeof(servAddr)); // Clear the socket address structure
    // Create an unamed socket, name will be bound in later
    sockFd = socket(PF_INET, SOCK_STREAM, 0); // Note PF_ not AF_

    if (sockFd == -1)
    {
        perror("Did not create the socket");
        exit(EXIT_FAILURE);
    }

    // Fill in the address structure
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(5000);

    if (inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) == -1)
    {
        perror("inet_pton problem");
        exit(EXIT_FAILURE);
    }

    // Name the socket by binding it to the servAddr structures data
    if (bind(sockFd, (struct sockaddr *)&servAddr, sizeof(servAddr)) == -1)
    {
        perror("Did not bind socket properly");
        close(sockFd);
        exit(EXIT_FAILURE);
    }

    // Set up 'listen' queue for up to 6 client connections
    if (listen(sockFd, 6) == -1)
    {
        perror("listen failure");
        close(sockFd);
        exit(EXIT_FAILURE);
    }
    // Server is now set up

    printf("\nI am the server starting up. Hit Ctrl + C to close me");
    printf("\nServer info: (IP: %s, port: %d)\n\n", inet_ntoa(servAddr.sin_addr), ntohs(servAddr.sin_port));

    // Now continuously loop accepting and processing client requests
    while (1)
    {
        pthread_t thread;                                       // thread
        connFd = accept(sockFd, (struct sockaddr *)NULL, NULL); // Wait for client and process it
        if (connFd < 0)
        {
            perror("Accept call failed");
            close(connFd);
            close(sockFd);
            exit(EXIT_FAILURE);
        }

        //Create a thread
        if ((rc = pthread_create(&thread, NULL, (void *)&ServerFunc, &connFd)) < 0)
        {
            printf("Error in creating thread %d\n", rc);
        }
    }

    close(sockFd);
    return (EXIT_SUCCESS);
}

// This server function
void *ServerFunc()
{

    char transmitBuff[1024] = {0};
    char receiveBuff[1024] = {0};
    // Set local connFd equal to current global connFd
    int threadConnFd = connFd;

    pthread_mutex_lock(&mutex1); // Lock the ciritcal section with the mutex
    printf("\nClient %d has been connected\n\n", client++);
    pthread_mutex_unlock(&mutex1); // Release the critical section
    sprintf(transmitBuff, "Hello client, the server is using this fd for you: %d\n", threadConnFd);

    //Send Fd word above to client
    if ((write(threadConnFd, transmitBuff, strlen(transmitBuff))) == -1)
    {
        perror("write to connection failure");
        close(threadConnFd);
        close(sockFd);
        exit(EXIT_FAILURE);
    }
    for (;;)
    {
        memset(receiveBuff, 0, sizeof(receiveBuff)); // clear the receive buffer
        sprintf(transmitBuff, "Enter \033[1;32m End \033[0m to end client program\nEnter word for Server:\n");

        //Send Fd word above to client
        if ((write(threadConnFd, transmitBuff, strlen(transmitBuff))) == -1)
        {
            perror("write to connection failure");
            close(threadConnFd);
            close(sockFd);
            exit(EXIT_FAILURE);
        }

        // Read from the connection
        if ((read(threadConnFd, receiveBuff, sizeof(receiveBuff))) == -1)
        {
            perror("read from connection failure");
            close(threadConnFd);
            close(sockFd);
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(&mutex2); // Lock the ciritcal section with the mutex
        printf("This server to date has an aggregate of %d reads for all clients\n", counter++);
        pthread_mutex_unlock(&mutex2); // Release the critical section

        printf("I just read this from a client: %s\n\n", receiveBuff);
        //If the Client nntered End Command close thread.
        if (strcmp(receiveBuff, "End\r\n") == 0)
        {
            //Notify client of termination
            sprintf(transmitBuff, "\nEnding connection between Client and server \nTime received word: %s\n\n", ctime(&t)); // Time into buffer

            // Write to the connection
            if ((write(threadConnFd, transmitBuff, strlen(transmitBuff))) == -1)
            {
                perror("write to connection failure");
                close(threadConnFd);
                close(sockFd);
                exit(EXIT_FAILURE);
            }
            printf("Closing down this client connection\n");
            close(threadConnFd); // Close the connection
            printf("I have just dealt with a client!\n \n");
            pthread_exit(NULL); // Thread exits
            break;              //exit out of for loop.
        }

        else
        {
            //Give time to transmit buffer
            t = time(NULL);                                                                                               // returns time in seconds since UNIX epoch
            sprintf(transmitBuff, "\nServer received the word: %s \nTime received word: %s\n\n", receiveBuff, ctime(&t)); // Time into buffer

            // Write to the connection
            if ((write(threadConnFd, transmitBuff, strlen(transmitBuff))) == -1)
            {
                perror("write to connection failure");
                close(threadConnFd);
                close(sockFd);
                exit(EXIT_FAILURE);
            }
        }
    }
}