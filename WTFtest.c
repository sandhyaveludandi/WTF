#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
    char* buffer = malloc(100);
 
    memset(buffer, 0, 100);
    sprintf(buffer, "./WTF configure localhost 10768");
    system(buffer);

    memset(buffer, 0, 100);
    sprintf(buffer, "./WTF create TestProject");
    system(buffer);

    memset(buffer, 0, 100);
    sprintf(buffer, "./WTF add TestProject TestFile");
    system(buffer);

    return 0;
}