#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include "create.h"

void error(char *msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[]) {
  int sockfd = -1;
  int newsockfd = -1;
  int portno = -1;
  int clientLength = -1;
  int n = -1; //utility variable - for monitoring reading/writing from/to the socket
  char *buffer; //char array to store data going to and coming from the server
  char *buffer2;

  struct sockaddr_in serverAddressInfo; //Super-special secret C struct that holds address info for building our socket
  struct sockaddr_in clientAddressInfo; //Super-special secret C struct that holds info about a machine's address

  //if user didn't enter enough arguments, exit
  if(argc < 2) {
    fprintf(stderr, "ERROR, no port provided\n");
    exit(0);
  }


  //If user provided sufficient arguments, obtain port number and address
  portno = atoi(argv[1]);


  //Build socket, if it doesn't work print error message and exit
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
   error("ERROR opening socket");

  /** Now we have the port to build our server socket on, so set up the address struct **/

  //zero out the socket address info struct and initialize it
  bzero((char*) &serverAddressInfo, sizeof(serverAddressInfo));

  //set a flag to indicate the type of network address we'll be using
  serverAddressInfo.sin_family = AF_INET;

  //set the remote port, translate from a 'normal' int to a super-special 'network-port-int'
  serverAddressInfo.sin_port = htons(portno);

  //set a flag to indicate the type of network address we'll be willing ot accept connections from
  serverAddressInfo.sin_addr.s_addr = INADDR_ANY;


  /** We have an address struct and a socket, build server socket **/

  //bind the server socket to a specific local port, so the client has a target to connnect to
  if(bind(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo)) < 0) {
    error("ERROR binding");
  }

  //set up the server socket to listen for client connections
  listen(sockfd, 0);

  //determine the size of a clientaddressInfo struct
  clientLength = sizeof(clientAddressInfo);


  //block until a client connects, when it does, create a client socket
  while(1)
    {
        newsockfd = accept(sockfd, (struct sockaddr *) &clientAddressInfo, &clientLength);
         //if the connection failed, throw error message and exit
        if(newsockfd < 0)
          error("ERROR on accept");

        /** Client tried to connect to server **/
          char buf[2];
          buf[1] = '\0';
          buffer = (char*)calloc(1, sizeof(char));
          char* pName;
          int numBytes;
          int isCreate = -1;


        //reading loop for number of bytes and command like "6:create;9:Project 1"
        while((n = read(newsockfd, &buf[0], sizeof(char))) > 0) {
          printf("buffer: %s\n", buffer); 
          if(buf[0] == ':') {
            numBytes = atoi(buffer);
            free(buffer);

            buffer = (char*)calloc(numBytes, sizeof(char));
            n = read(newsockfd, buffer, numBytes);
            //printf("Buffer: %s\n", buffer);

            //if failed throw error and exit
            if(n < 0) {
              error("ERROR reading from filedescriptor");
            }

            if(strcmp(buffer, "create") == 0) {
              isCreate = 1;
            }

            if(isCreate == 1) {
              pName = (char*)calloc(numBytes, sizeof(char));
              strcpy(pName, buffer);
            }

            free(buffer);
            buffer = (char*)calloc(1, sizeof(char));

        }
        else if(buf[0] == ';') {
          continue;
        }
        else {
          strcat(buffer, buf);
          //printf("Added Buffer: %s\n", buffer);
        }
    }
     printf("Yohoo\n");
     free(buffer);
     buffer = (char*)calloc(1, sizeof(char));
     //Create directory within serverFolder called Project 1
      if(isCreate == 1) {
        printf("Directory created within folder.\n");
        //write to server if success or failure
        if(createFunction(newsockfd, pName) == 0) {
          printf("Directory successfully created.\n");
          strcat(buffer, "Successfully created folder on server!\n");
        } else {
          printf("Failure\n");
          strcat(buffer, "ERROR: Unable to create folder on server.\n");
        }
        n = write(newsockfd, buffer, strlen(buffer));
        if(n < 0) {
          error("ERROR: reading from filedescriptor");
        }
        printf("Message written to client.\n");
      }

      //free(buffer);
      sleep(1);
    }

  close(newsockfd);
  return 0;
}
