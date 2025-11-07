#include <stdio.h>

struct FileInformation{
    char version[10];
    char author[11];
    char name[10];
    char year[4];
};
const struct FileInformation fileInformation = {
    .version = "1.0",
    .author = "Petr Moucha",
    .name = "noteEditor",
    .year = "2025"
};

void helpGuide(){
    for (int i=0; i<40; i++){printf("=");}
    printf("\nThis is help guide for %s program\n", fileInformation.name);
    for (int i=0; i<40; i++){printf("=");}
    printf("   Author: %s\n", fileInformation.author);
    printf("   Version: %s\n", fileInformation.version);
    printf("   Written in %s\n", fileInformation.year);
    for (int i=0; i<30; i++){printf("-");}
    printf("\n\n%5s %5s\n", "-h", "print (this) help quied");
    printf("%5s %15s\n", "-v", "print version");
    printf("%5s %15s\n", "-w [date]", "write new line to file with optional parameter [date], which writes date to second column");
    printf("%5s %15s\n", "-d [number]", "delete line [number]");
    printf("%5s %15s\n", "-e [number]", "TODO"/*"print editable text from line [number] and save it edited back on line"*/);
    printf("%5s %15s\n\n", "-s [number]", "sort lines 0 - in the order of writing, 1 - by date in second column, 2 - alphabetical");
    printf("For changes and new versions (if a new version ever comes) look for TODO-github.");
}

int main(int argc, char *arg[]){
    if (argc>1){
        for (int i=0; i<argc; i++){
            if (arg[i][0]=='-'){
                switch (arg[i][1]){
                case 'h':
                    helpGuide();
                    break;
                case 'w':
                    break;
                case 'd':
                    break;
                case 'e':
                    break;
                case 's':
                    break;
                case 'v':
                    printf("%s\n", fileInformation.version);
                    break;
                default:
                    printf("invalid argument -%c", arg[i][1]);
                    break;
                }
            }
        }
    }
    else{
        printf("missing arguments (type -h for help)");
    }

    return 0;
}