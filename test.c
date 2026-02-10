#include <stdio.h>
#include <stdlib.h>

int main(){
    printf("%*d) %s %*s\n",3, 12, "Ahoj svete", 15, "11-12-2024");
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