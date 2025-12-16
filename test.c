#include <stdio.h>

int main(){
    FILE* write = fopen("./test.txt", "w");
    fprintf(write,"puvodni veta.\n");
    fclose(write);
    FILE* read = fopen("./test.txt", "r+");
    char a='a';
    while (a!='\n'){
        fscanf(read, "%c", &a);
        printf("%c ", a);
    }
    fputc(' ', read);
    fprintf(write, "Druhy radek\n");
    fclose(write);
    return 0;
}