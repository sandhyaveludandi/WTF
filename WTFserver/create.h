#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>


int createFunction(int conn, char *projName)
{
  struct stat buf;
  int exists;
  DIR *d;
  struct dirent *de;

  d = opendir(".");
  if (d == NULL) {
    fprintf(stderr, "Couldn't open \".\"\n");
    exit(1);
  }

  for (de = readdir(d); de != NULL; de = readdir(d)) {
    exists = stat(de->d_name, &buf);
    if (exists < 0) {
      fprintf(stderr, "%s not found\n", de->d_name);
    } else {
      //printf("%s %lld\n", de->d_name, buf.st_size);
      if(strcmp(de->d_name, projName) == 0) {
          printf("ERROR: File or directory name %s already exists in this directory\n", projName); 
      }
    }
  }

  //create a directory with projName
  int status = mkdir(projName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if(status < 0) {
      printf("ERROR: Directory could not be created.\n"); 
      exit(1); 
  }

  char *filepath = malloc(strlen(projName) + strlen(".Manifest") + 2);
  filepath = strcpy(filepath, projName);
  filepath = strcat(filepath, "/.Manifest");

  //create a .Manifest file inside the new directory 
  int m = open(filepath, O_RDWR| O_CREAT, S_IRUSR | S_IWUSR);
  if(status < 0) {
      printf("ERROR: Manifest file could not be created.\n"); 
      exit(1); 
  }

  close(m); 
  closedir(d);
  return 0;
}