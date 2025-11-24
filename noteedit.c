#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//return value 
/*
0 - program runs succesfully
1 - terminated by user
*/

//--------------------
//TODO list
//--------------------
/*
predelat nacitani a kontrolu. Nacist oboji zvlast - zkontrolovat platnost a v pripade konfliktu nabidnou reseni
nacitat idealne ve stejne funkci

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

int errorQuestion(){
    printf("Some data can be lost. Do you want to continue? [Y/n]");
    int c;
    scanf("%c\n",&c);
    if (c=='Y') return 0;
    else{
        pritnf("Proccess canceled...\n");
        return 1;
    }
}

//default settings are overwritten from noteeditset.txt
//sort - 0 - in order of writing, 1 - by date, 2 - alphabetical | default value 1
//dateFormat - TODO
//line length - limit for length of text to fit in conky window
//rows - limit for number of rows
const int stprm = 4;//number of parameters in settings
const int nameLength = 20;//max length of parameters in settings 19+'\0'
const int valueLen = 4;//max digits of values of prameters 3+'\0'
struct Value{
    char sort;
    char dateFormat;
    int lineLength;
    int rows;
};
struct Alias{
    char sort[nameLength];
    char dateFormat[nameLength];
    char lineLength[nameLength];
    char rows[nameLength];
};
struct Settings{
    struct Value value;
    struct Alias alias;
};
struct Settings settings = {
    .value.sort = '0',
    .value.dateFormat = '0',
    .value.lineLength = 60,
    .value.rows = 50,
    .alias.sort = "sortBy",
    .alias.dateFormat = "dateFormat",
    .alias.lineLength = "lineLength",
    .alias.rows = "rows"
};
struct ProcessInformation{
    int notesLines;
    int longestLine;
    short loaded;//settings and notes
};
struct ProcessInformation procInfo = {
    .loaded = 0
};

int loadSettings(FILE *ptr){
    char c;
    char keyword[stprm][nameLength];//19 characters of name
    char value[stprm][valueLen];//3 digits +'\0'
    int loadObject = 0;//0-keyword, 1-value
    int invalidChracter = 0;
    int word;
    int ic;
    while(fscanf(ptr, "%c", &c)!=EOF){
        if (loadObject==0){//load keyword
            if (ic>=nameLength-1){//20th character reserved for '\0'
                printf("Too long keyword - check settings file or increase nameLength(=%d) in the script.\n", nameLength-1);
                if (errorQuestion()==1) return 1;

                int status=0;
                while(c!='\n'&&status!=EOF&&c!=' '){//read and discard all other characters on line
                    status= fscanf(ptr, "%c", &c);
                }
                if (status==EOF) break;
                if (c=='\n'){//too long keyword ending by \n
                    keyword[word][ic]='\0';
                    printf("Missing value in settings after keyword %s. Can be repaced by default value.\n", keyword[word]);
                    if (errorQuestion()==1) return 1;
                    ic=0;
                    value[word++][ic]='\0';//empty string - if keyword existi, it will be later set on default value
                }
                else{
                    keyword[word][ic]='\0';
                    ic=0;
                    loadObject=1;
                }
            }
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
                printf("Missing value in settings after keyword %s. Can be repaced by default value.\n", keyword[word]);
                if (errorQuestion()==1) return 1;
                ic=0;
                value[word++][ic]='\0';//empty string - if keyword existi, it will be later set on default value
            }
            //else the char is ' ' before some word or '\n' on the beginning of line and it is ignored
        }
        else{
            if (ic>=valueLen-1){//4th character reserved for '\0'
                printf("Too long value - check settings file or increase valuenLen(=%d) in the script.\n", valueLen-1);
                if (errorQuestion()==1) return 1;
                int status=0;
                while(c!='\n'&&status!=EOF){
                    status= fscanf(ptr, "%c", &c);
                }
                if (status==EOF) break;
                value[word++][ic]='\0';
                ic=0;
            }
            if (c=='\n'&&ic!=0){//end of value
                if (invalidChracter) {
                    printf("Invalid character/s in settings after keyword %s. Right format-[keyword]SPACE[number]ENTER\n", keyword[word]);
                    if (errorQuestion()==1) return 1;
                }
                invalidChracter=0;
                value[word++][ic]='\0';
                ic = 0;
                loadObject = 0;
            }
            else if (c=='\n'&&ic!=0){
                printf("Missing value in settings after keyword %s. Can by replaced by default value.\n", keyword[word]);
                if (errorQuestion()==1) return 1;
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
        printf("Missing value in settings after keyword %s. It can be deleted.\n", keyword[word]);//ending without enter after keyword
        if (errorQuestion()==1) return 1;
    }
    else if (ic !=0){
        value[word][ic]='\0';//ending after value without enter
    }
    else if (loadObject==1){
        value[word][0]='\0';
        printf("Missing value in settings after keyword %s. It can be deleted.\n", keyword[word]);//ending without enter after keyword and space
        if (errorQuestion()==1) return 1;
    }

    for (int i=0; i<stprm; i++){
        //every value is already checked if it is 3 digits in string or \0
        if (strcmp(keyword[i], settings.alias.dateFormat)==0){
            //dateFormat valid values TODO
            if (value[i]=="") continue;//in all cases for outputing unknown keyword even without value
            if (value[i][0]<='2')
                settings.value.dateFormat = value[i][0];
        }
        else if (strcmp(keyword[i], settings.alias.lineLength)==0){
            if (value[i]=="") continue;
            int num = atoi(value);
            //kontorola delky ulozenych stringu v notes
            if (num<1){
                printf("Length of line must be positive number. It will be set on default value.\n");
                if (errorQuestion()==1) return 1;
            }
            else if (num<14){
                printf("Warning! No space for dates.");//missing space
                settings.value.lineLength = num;
            }
            else settings.value.lineLength = num;
        }
        else if (strcmp(keyword[i], settings.alias.rows)==0){
            if (value[i]=="") continue;
            int num = atoi(value);
            //kontorola poctu radku v notes
            if (num<1){
                printf("Number of lines must be positive number. It will be set on default value.\n");
                if (errorQuestion()==1) return 1;
            }
            else settings.value.rows = num;
        }
        else if (strcmp(keyword[i], settings.alias.sort)==0){
            if (value[i]=="") continue;
            if (value[i][0]=='0'||value[i][0]=='1'||value[i][0]=='2'){
                settings.value.sort=value[i][0];
            }
            else{
                printf("In settings is invalid value after keyword %s. It will be set on default value.s\n", keyword[i]);
                if (errorQuestion()==1) return 1;
            }
        }
        else{
            printf("In settings is unknown keyword %s.\n", keyword[i]);
            if (errorQuestion()==1) return 1;
        }
    }
}
void writeSettings(FILE *ptr){
    fprintf(ptr, "%s %s\n", settings.alias.sort, settings.value.sort);
    fprintf(ptr, "%s %s\n",settings.alias.dateFormat, settings.value.dateFormat);
    fprintf(ptr, "%s %s\n",settings.alias.lineLength, settings.value.lineLength);
    fprintf(ptr, "%s %s\n",settings.alias.rows, settings.value.rows);
}
void sort(){

}
int loadSavedData(FILE *fptr, char line[settings.value.rows][settings.value.lineLength], char date[settings.value.rows][10]){
    char chr;
    int ichr = 0;
    int iword = 0;
    int textpart = 0;//true after "X) "
    while(fscanf(fptr, "%c", &chr)!=EOF){
        if (textpart==1){//first characters are "X) " - ignore them
            if (chr=='\n') {//end of line/note icrease index for load of another line
                if (ichr==settings.value.lineLength){//if line use all of its space last 10 characters must by date dd-mm-yyyy and '\n'
                    for (int j=settings.value.lineLength-11; j<settings.value.lineLength; j++){
                        date[iword][j-settings.value.lineLength-11]=line[iword][j];
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
char* delete(char* lineNum){
    
}
int write(int argc, char arg[], int i){

}

void setSort(char sortBy){
    FILE *fptrnotes;
    FILE *fptrset;
    char line[settings.value.rows][settings.value.lineLength];
    char date[settings.value.rows][10];

    if (fopen("~/notes.txt", "r")!=NULL){
        fptrnotes = fopen("~/notes.txt", "r");
    }
    else{
        fclose(fopen("~/notes.txt", "a"));
        procInfo.longestLine=0;
        procInfo.notesLines=0;
    }
    if (fopen("~/noteeditset.txt", "r")!=NULL)
        fptrset = fopen("~/noteeditset.txt", "r");
    else{
        settings.value.sort = sortBy;
        fptrset = fopen("~/noteeditset.txt", "a");//TODO
    }
    fclose(fptrnotes);
}
int setEdit(int lenght, char line[]){

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
    short argArrLen = 7;
    short argMaxLen = 4;
    char arguments[] = {
        "-h", "-d", "-w",
        "-e", "-sr", "-st",
        "-v"
    };
    if (argc>1){
        for (int i=1; i<argc; i++){//ignore argument with path of script
            if (arg[i][0]=='-'){
                for (int j=0; j>argArrLen; j++){
                    if (strcmp(arg[i], arguments[j])==0)
                        switch (j){
                        case 0: helpGuide();break;
                        case 1: 
                            if(argc>i+1 && arg[i+1][0]!='-')
                                if(delete(arg[++i])==NULL) printf("Line %d does not exist\n", arg[i]);
                            else printf("Missing argument after command %s.\n", arg[i]);
                            break;
                        case 2: i+=write(argc, arg, i); break;
                        case 3: 
                        case 4:
                        case 5:
                        case 6:
                        }
                }
            }
            else printf("invalid argument %s (type -h for help)\n", arg[i]);
        }
    }
    else printf("missing arguments (type -h for help)\n");

    return 0;
}
