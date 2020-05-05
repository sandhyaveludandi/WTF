#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "configure.h"
#include "create.h"
#include "add.h"

typedef struct argum {
  int argc;
  char *ip;
  char *port;
  char** argv;
}argum;

void error(char *msg) {
  perror(msg);
  exit(0);
}

void tostring(char str[], int num) {
    int i, rem, len = 0, n;
    n = num;
    while (n != 0) {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++) {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void* clientThread(void *arg) {
  //converting void pointer back into a argum struct
  argum* num = (argum*)arg;
  int argc = num->argc;
  char *ipAdd = (char*)malloc(strlen(num->ip)*sizeof(char));
  char *po = (char*)malloc(strlen(num->port)*sizeof(char));
  char** argv = (char**)calloc(argc, sizeof(char*));

  strcpy(ipAdd, num->ip);
  strcpy(po, num->port);

  int i;
  for(i = 1; i < argc; i++) {
    argv[i] = (char*)calloc(strlen(num->argv[i]), sizeof(char));
    argv[i] = num->argv[i];
  }

  //server sokcet setup
  int sockfd = -1;
  int portno = -1;
  int n = -1; //utility variable - for monitoring reading/writing from/to the socket
  char *buffer; //char array to store data going to and coming from the server
  struct sockaddr_in serverAddressInfo; //Super-special secret C struct that holds address info for building our socket
  struct hostent *serverIPAddress; //Super-special secret C struct that holds info about a machine's address
  //If user provided sufficient arguments, obtain port number and address
  portno = atoi(po);

  //Look up IP address that matches up with the name given
  //The name given might be an IP address, which is fine, and store it in the
  //'serverIPAddress' struct
  serverIPAddress = gethostbyname(ipAdd);
  if(serverIPAddress == NULL) {
    fprintf(stderr, "ERROR, host %s does not exist: Please configure with a proper host.\n", ipAdd);
    exit(0);
  }

  //Build socket, if it doesn't work print error message and exit
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0)
    error("ERROR creating socket");

  /**Now we have the IP address and port to connect to on the servwer, we have to get **/
  /** that information into C's special address struct for connnecting sockets **/
  //zero out the socket address info struct and initialize it
  bzero((char*) &serverAddressInfo, sizeof(serverAddressInfo));

  //set a flag to indicate the type of network address we'll be using
  serverAddressInfo.sin_family = AF_INET;

  //set the remote port, translate from a 'normal' int to a super-special 'network-port-int'
  serverAddressInfo.sin_port = htons(portno);

  //do a raw copy of the bytes that represent the server's IP Address in
  //the 'serverIPAddress' struct into our serverIPAddressInfo struct
  bcopy((char*)serverIPAddress->h_addr_list[0], (char*)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);

  /*Now we have a blank socket and fully parametrized address info struct, now we connect **/

  //Connect to server using blank socket adn the address info struct
  //if it doesn't work, throw error message and exit

  //create
  int conn = connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo));
  //if the user has not set con
  if(conn < 0) {
    error("ERROR connecting: Configure port number properly.");
  }

  /** Client is connected to server **/
  printf("Client is now connected to server\n"); 
  /**CREATE COMMAND**/
  //argv[1] is create
  if(strcmp(argv[1], "create") == 0 ) {
    //Send the number of bytes needed for create (6)
    char hold[4];
    hold[3] = '\0';
    //hold has the length of the project name as a string
    tostring(hold, strlen(argv[2]));
    //printf("Hold: %s\n", hold);
    strcat(hold, ":");
    strcat(hold, argv[2]);

    //printf("Hold: %s\n", hold);
    char *str = (char*)calloc(strlen(hold)+8, sizeof(char));
    strcat(str, "6:create;");
    strcat(str, hold);
    //printf("Hold: %s\n", str);

    n = write(sockfd, str, strlen(str));

    //If writing to server failed, throw error message and exit
    if(n < 0) {
      error("ERROR writing to socket");
    }
    //create the project directory and .Manifest file after the server has
    createFunction(conn, argv[2]);

  } else if(strcmp(argv[1], "add") == 0 ) {
      //argv[2]: project name, argv[3]; file to e added
      addFunction(argv[2], argv[3]);
  }

  close(sockfd);
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  char *ip;
  char *port;
   //printf("%s\n", argv[1]);
  //Checks if configurated properly
  if(strcmp(argv[1], "configure") == 0) {
    if(configureFunction(argc, argv) < 0) {
      printf("ERROR: Configure failed\n");
      exit(1);
    }
    printf("Configured Successfully!\n");
    return 1;
  }
  //if command is not configure, check if configurated already
  //read from the .configure file to get the ip and port
  else {
    char *buffer = (char*)calloc(1, sizeof(char));
    char buf[2];
    buf[1] = '\0';
    int n;
    int fd = open(".configure", O_RDONLY);
    if(fd < 0) {
      error("ERROR: .configure has not been created or could not be opened.\n");
    }
    while((n = read(fd, &buf[0], sizeof(char))) > 0) {
      if(buf[0] == ' ') {
        ip = (char*)malloc(strlen(buffer)*sizeof(char));
        strcpy(ip, buffer);
        free(buffer);
        buffer = (char*)calloc(1, sizeof(char));
      }  else {
        strcat(buffer, buf);
        //printf("Added Buffer: %s\n", buffer);
      }
    }
    port = (char*)malloc(strlen(buffer)*sizeof(char));
    strcpy(port, buffer);
    free(buffer);
  }
   //Checks if create command is used
  if(strcmp(argv[1], "create") == 0) {
        if(argc != 3) {
            printf("ERROR: Wrong number of args for create command.\n");
            exit(1);
        }
     //printf("Create command used\n");
  } else if(strcmp(argv[1], "update") == 0) {
    //printf("hello\n");
        if(argc != 5) {
            printf("ERROR: Wrong number of args for update command.\n");
            exit(1);
        }
     //printf("Create command used\n");
  } else if(strcmp(argv[1], "add")==0){
    if(argc != 4){
        printf("Error: Wrong number of args for add command.\n");
        return -1;
    }
    printf("Adding file\n");
  } else {
    printf("Please enter a valid command.\n");
    return 1;
  }

  argum* arg = (argum*)malloc(sizeof(argum));
  arg->argc = argc;
  arg->ip = (char*)malloc(strlen(ip)*sizeof(char));
  arg->port = (char*)malloc(strlen(port)*sizeof(char));
  arg->argv = (char**)malloc(argc*sizeof(char*));

  strcpy(arg->ip, ip);
  strcpy(arg->port, port);

  int i;
  for(i = 0; i < argc; i++) {
    arg->argv[i] = (char*)malloc(strlen(argv[i])*sizeof(char));
    arg->argv[i] = argv[i];
  }

  pthread_t t;
  int res;
  res = pthread_create(&t, NULL, clientThread, (void *) arg);
  if(res < 0) {
    error("ERROR thread could not be created");
  }

  pthread_exit(NULL);
  return 0;
}
