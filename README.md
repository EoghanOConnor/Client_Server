# Client_Server
Client Server demonstration

Includes 2 programme files myClient.c and myServer.c

MyClient.c
File Description: This is the myClient.c file.
                  The client gets the address to the socket
                  The client connects to the socket
                  The the client prompts user for input
                  The client sends input to server.
                  If the word say "End" the connection with client is closed
                  and the client program closes down.
 
MyServer2.c
File Description: This is the myServer.c file.
                  The server creates a socket
                  The server reads the word from the Socket connection.
                  The server sends back the fd connection to client.
                  The server then sends the word back to the client.
                  The server sends back a time stamp of when the word was recieved
                  If the word say "End" the connection with client is closed
                  
                  
