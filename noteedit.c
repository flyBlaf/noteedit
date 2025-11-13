#include <stdio.h>
#include <string.h>

//--------------------
//TODO list
//--------------------
/*
github
date formates
excec command in conky .conf
editation of line
*/

struct FileInformation{
    char version[10];
    char author[12];
    char name[11];
    char year[5];
};
const struct FileInformation fileInformation = {
    .version = "1.0",
    .author = "Petr Moucha",
    .name = "noteEditor",
    .year = "2025"
};

void helpGuide(){
    for (int i=0; i<60; i++){printf("=");}
    printf("\nThis is help guide for %s program\n", fileInformation.name);
    for (int i=0; i<60; i++){printf("=");}
    printf("\n   Author: %s\n", fileInformation.author);
    printf("   Version: %s\n", fileInformation.version);
    printf("   Written in %s\n", fileInformation.year);
    for (int i=0; i<30; i++){printf("-");}
    printf("\n\nThis program formating text to text file for right visualization by conky.\nTo .conf write {TODO-formating by conky}\nText is separate to two columns 1.subject 2.Date and default sort is by this date.\nLines without date are in the order of writing");
    printf("\n\n   %-18s %s\n", "-h", "print (this) help quied");
    printf("   %-18s %s\n", "-v", "print version");
    printf("   %-18s %s\n", "-w [text] [date]", "write new line to file with optional parameter [date] in format dd-mm-yyyy, which writes date to second column");
    printf("   %-18s %s\n", "-d [number]", "delete line [number]");
    printf("   %-18s %s\n", "-e [number]", "TODO"/*"print editable text from line [number] and save it edited back on line"*/);
    printf("   %-18s %s\n\n", "-s [number]", "sort lines 0 - in the order of writing, 1 - by date in second column, 2 - alphabetical");
    printf("For changes and new versions (if a new version ever comes) look for TODO-github.\n");
}

//default settings are overwritten form noteeditset.txt
//sort - 0 - in order of writing, 1 - by date, 2 - alphabetical | default value 1
//dateFormat - TODO
//line length - limit for length of text to fit in conky window
//rows - limit for number of rows
struct Settings{
    char sort;
    char dateFormat;
    int lineLength;
    int rows;
    char sortAlias[20];
    char formatAlias[20];
    char lineLengthAlias[20];
    char rowsAlias[20];
};
struct Settings settings = {
    .sort = '0',
    .dateFormat = '0',
    .lineLength = 60,
    .rows = 50,
    .sortAlias = "sortBy",
    .formatAlias = "dateFormat",
    .lineLengthAlias = "lineLength",
    .rowsAlias = "rows"
};
void loadSettings(FILE *ptr){
    char c;
    char keyword[10][20];//19 characters of name
    char value[10][4];//xxx+'\0'
    int loadObject = 0;//0-keyword, 1-value
    int invalidChracter = 0;
    int word;
    int ic;
    while(fscanf(ptr, "%c", &c)!=EOF){
        if (loadObject==0){
            if (c==' '&&ic!=0){//end of keyword
                keyword[word][ic]='\0';
                ic = 0;
                loadObject = 1;
            }
            else if (c!=' ' && c!='\n'){
                keyword[word][ic++] = c;
            }
            else if (c=='\n'&&ic!=0){
                keyword[word][ic]='\0';
                printf("missing value in settings after keyword %s\n", keyword[word]);
                ic=0;
                value[word++][ic]='\0';//empty string - if keyword existi, it will be later set on default value
            }
            //else the char is ' ' before some word or '\n' on the beginning of line and it is ignored
        }
        else{
            if (c=='\n'&&ic!=0){//end of value
                if (invalidChracter) printf("invalid character/s in settings after keyword %s. Right format-[keyword]SPACE[number]ENTER\n", keyword[word]);
                invalidChracter=0;
                value[word++][ic]='\0';
                ic = 0;
                loadObject = 0;
            }
            else if (c=='\n'&&ic!=0){
                printf("missing value in settings after keyword %s\n", keyword[word]);
                ic=0;
                value[word++][ic]='\0';//empty string - if keyword existi, it will be later set on default value
            }
            else if (c>'0' && c<'9'){//load only numbers
                value[word][ic++]=c;
            }
            else invalidChracter = 1;//all characters which are not '\n' or numbers
        }
    }
    if (ic != 0 && loadObject == 0){
        keyword[word][ic]='\0';
        value[word][0]='\0';
        printf("missing value in settings after keyword %s\n", keyword[word]);//ending without enter after keyword
    }
    else if (ic !=0){
        value[word][ic]='\0';//ending after value without enter
    }
    else if (loadObject==1){
        value[word][0]='\0';
        printf("missing value in settings after keyword %s\n", keyword[word]);//ending without enter after keyword and space
    }

    //prepsat z keyword a value do promenych
}
void writeSettings(FILE *ptr){
    fprintf(ptr, "%s %s\n", settings.sortAlias, settings.sort);
    fprintf(ptr, "%s %s\n",settings.formatAlias, settings.dateFormat);
    fprintf(ptr, "%s %s\n",settings.lineLengthAlias, settings.lineLength);
    fprintf(ptr, "%s %s\n",settings.rowsAlias, settings.rows);
}
void sort(){

}
void load(FILE *fptr, char line[settings.rows][settings.lineLength], char date[settings.rows][10]){
    char chr;
    int ichr = 0;
    int iword = 0;
    int textpart = 0;//true after "X) "
    while(fscanf(fptr, "%c", &chr)!=EOF){
        if (textpart==1){//first characters are "X) " - ignore them
            if (chr=='\n') {//end of line/note icrease index for load of another line
                if (ichr==settings.lineLength){//if line use all of its space last 10 characters must by date dd-mm-yyyy and '\n'
                    for (int j=settings.lineLength-11; j<settings.lineLength; j++){
                        date[iword][j-settings.lineLength-11]=line[iword][j];
                    }
                }
                iword++;
                ichr = 0;
                textpart = 0;
            }
            else line[iword][ichr] = chr;
        }
        else if (chr==' ') textpart = 1;
        ichr++;
    }
}


void setSort(char sortBy){
    FILE *fptrnotes;
    FILE *fptrset;
    if (fopen("~/noteeditset.txt", "r")!=NULL)
        fptrset = fopen("~/noteeditset.txt", "r");
        
    else{
        settings.sort = sortBy;
        fptrset = fopen("~/noteeditset.txt", "a");//TODO
    }

    if (fopen("~/notes.txt", "r")!=NULL)
        fptrnotes = fopen("~/notes.txt", "r");
    else{
        fclose(fopen("~/notes.txt", "a"));
        return;
    }

    char line[settings.rows][settings.lineLength];
    char date[settings.rows][10];
    load(fptrnotes, line, date);

    fclose(fptrnotes);
}
void setEdit(int lenght, char line[]){

}
void setWrite(char text[], char date[]){

}
void delete(int lenght, char line[]){
    
}

//noteeditset.txt - saved settings
/*
sortBy 1
dateFormat 1 *yet not implemented
lineLength 60
rows 50
*/
//notes.txt - this is printed
/*
1) text1                      01-01-2025
2) text2                      02-03-2025
3) text3                      10-06-2025
*/

int main(int argc, char *arg[]){
    if (argc>1){
        for (int i=1; i<argc; i++){
            if (arg[i][0]=='-'){
                switch (arg[i][1]){
                case 'h':
                    helpGuide();
                    break;
                case 'w':
                    if (argc>i+2){//exist next two arguments [text] [date]
                        setWrite(arg[i+1], arg[i+2]);
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
                        setEdit(strlen(arg[i+1]), arg[i+1]);
                        i++;
                    } else printf("missing argument after -%c\n", arg[i][1]);
                    break;
                case 's':
                    if (argc>i+1){//exists next argument
                        if (arg[i+1][0]=='0' || arg[i+1][0]=='1' || arg[i+1][0]=='2') {//call if next argument is valid (ignores continue of string)
                            setSort(arg[i+1][0]);
                            i++;
                        }
                        else printf("invalid argument after -%c (0/1/2)\n", arg[i][0]);
                    } else printf("missing argument after -%c\n", arg[i][1]);
                    break;
                case 'v':
                    printf("noteEditor %s\n", fileInformation.version);
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
