#include <stdio.h>
#include <string.h>

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
    printf("\n\nThis program formating text to text file for right visualization by conky.\nTo .conf write {TODO-formating by conky}\nText is separate to two columns 1.subject 2.Date and default sort is by this date. Lines without date are in the order of writing");
    printf("\n\n%5s %20s\n", "-h", "print (this) help quied");
    printf("%5s %20s\n", "-v", "print version");
    printf("%5s %20s\n", "-w [text] [date]", "write new line to file with optional parameter [date] in format dd-mm-yyyy, which writes date to second column");
    printf("%5s %20s\n", "-d [number]", "delete line [number]");
    printf("%5s %20s\n", "-e [number]", "TODO"/*"print editable text from line [number] and save it edited back on line"*/);
    printf("%5s %20s\n\n", "-s [number]", "sort lines 0 - in the order of writing, 1 - by date in second column, 2 - alphabetical");
    printf("For changes and new versions (if a new version ever comes) look for TODO-github.");
}
void sort(char sortBy){

}
void edit(int lenght, char line[]){

}
void write(char text[], char date[]){

}
void delete(int lenght, char line[]){
    
}

//ntedtset.txt - saved settings
/*
sortBy 1
dateFormat 1 *yet not implemented
*/
//notes.txt - this is printed
/*
1) text1                      1.1.2025
2) text2                      2.3.2025
3) text3                     10.6.2025
*/

int main(int argc, char *arg[]){
    if (argc>1){
        for (int i=0; i<argc; i++){
            if (arg[i][0]=='-'){
                switch (arg[i][1]){
                case 'h':
                    helpGuide();
                    break;
                case 'w':
                    if (argc>i+2){//exist next two arguments [text] [date]
                        write(arg[i+1], arg[i+2]);
                    } else printf("missing argument after -%c\n", arg[i][1]);
                    break;
                case 'd':
                    if (argc>i+1){//exists next argument
                        delete(strlen(arg[i+1]), arg[i+1]);
                        i++;
                    } else printf("missing argument after -%c\n", arg[i][1]);
                    break;
                case 'e':
                    if (argc>i+1){//exists next argument
                        edit(strlen(arg[i+1]), arg[i+1]);
                        i++;
                    } else printf("missing argument after -%c\n", arg[i][1]);
                    break;
                case 's':
                    if (argc>i+1){//exists next argument
                        if (arg[i+1][0]=='0' || arg[i+1][0]=='1' || arg[i+1][0]=='2') {//call if next argument is valid (ignores continue of string)
                            sort(arg[i+1][0]);
                            i++;
                        }
                        else printf("invalid argument after -%c (0/1/2)\n", arg[i][0]);
                    } else printf("missing argument after -%c\n", arg[i][1]);
                    break;
                case 'v':
                    printf("%s\n", fileInformation.version);
                    break;
                default:
                    printf("invalid argument -%c (type -h for help)\n", arg[i][1]);
                    break;
                }
            }
            else printf("invalid argument %s (type -h for help)\n", arg[i]);
        }
    }
    else printf("missing arguments (type -h for help)\n");

    return 0;
}
