#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    printf("%*d) %s%*c%s\n",3, 12, "Ahoj svete", 15, '|',"11-12-2024");
    short a;
    int b = 3;
    a = b;
    printf("%d\n", a);
    //printf("% *d) % *s|%s\n", 3, 12, "Ahoj svete", 15, "11-03-2001");
    //printf("% *d) % *s|%s\n", paddingStart, line_index, paddingDate, procInfo.text[line_index], procInfo.dates[line_index]);
    //char * str = "";
    //printf("%ld", strlen(str));
    //FILE * fptr = fopen("~/noteeditst.txt", "r");
    //if (fptr == NULL) printf("NULL");
    // printf("%d", atoi("+24_3_5"));
    // FILE* write = fopen("./test.txt", "w");
    // fprintf(write,"puvodni veta.\n");
    // fclose(write);
    // FILE* read = fopen("./test.txt", "r+");
    // char a='a';
    // while (a!='\n'){
    //     fscanf(read, "%c", &a);
    //     printf("%c ", a);
    // }
    // fputc(' ', read);
    // fprintf(write, "Druhy radek\n");
    // fclose(write);
    return 0;
}