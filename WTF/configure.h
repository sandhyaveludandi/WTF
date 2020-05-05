#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

char* LOCALHOST;
char* PORTNO;

int configureFunction(int argc, char* argv[]) {
        remove(".configure"); 
        if(argc != 4) {
            printf("Error: Wrong number of args for configure command.\n");
            return -1;
        }

        int c = open(".configure", O_RDWR| O_CREAT, S_IRUSR | S_IWUSR);
        if(c < 0) {
            printf("Error: File descriptor c could not be opened.\n");
            return -1;
        }

        //localhost
        LOCALHOST = (char*)malloc(strlen(argv[2])*sizeof(char));
        strcpy(LOCALHOST, argv[2]);
        int n = write(c, LOCALHOST, strlen(LOCALHOST));
        if(n < 0) {
            printf("Error: could not write to file.\n");
            return -1;
        }
        n = write(c, " ", 1);
        if(n < 0) {
            printf("Error: could not write to file.\n");
            return -1;
        }

        //port number
        PORTNO = (char*)malloc(strlen(argv[3])*sizeof(char));

        strcpy(PORTNO, argv[3]);
        n = write(c, argv[3], strlen(argv[3]));
        if(n < 0) {
            printf("Error: could not write to file.\n");
            return -1;
        }
        close(c);

    return 1;

}
