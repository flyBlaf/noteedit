#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(){
    FILE * fptr = fopen("/home/petr/test.txt", "w");
    fprintf(fptr, "test");

    return 0;
}
