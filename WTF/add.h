#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include<errno.h>

int addFunction(char* projName, char* fileName){
    //open project directory
    //printf("in add\n");
    //printf("Project Name: %s\n", projName);
    //printf("File Name: %s\n", fileName);
    DIR* d = opendir(projName);
    if(d==NULL){
        fprintf(stderr, "ERROR: Project folder name does not exist or project could not be open.\n");
        exit(1);
    }
    //open manifest file
    int manfd = 0;
    char* path = (char*)malloc((strlen(projName)+strlen("/.Manifest")) * sizeof(char));
    strcpy(path, projName);
    strcat(path, "/.Manifest");
    //printf("File Path: %s\n", path);
    manfd = open(path, O_RDWR);
    if(manfd < 0){
        printf("Error: .Manifest could not be opened.\n");
        exit(1);
    }
    //write fileName to .Manifest
    int w;
    w = write(manfd, fileName, strlen(fileName));
    if(w < 0){
        printf("Could not write to .Manifest\n");
        exit(1);
    }
    free(path);
    closedir(d);
    close(manfd);
    return 1;
}
