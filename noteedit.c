#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//return value 
/*
0 - program runs succesfully
1 - terminated by user
100 - saved lines in notes are too long to load into memory
101 - length of dates doesnt match with example
*/

//--------------------
//TODO list
//--------------------
/*
    zkontrolovat indexovani od 0
    write predelat na cisty prepis
    load_settings
    load_notes
    github
    date formates
    exec command in conky .conf
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
    printf("\n\n   %-21s %s\n", "-h", "print (this) help guied");
    printf("   %-21s %s\n", "-v", "print version");
    printf("   %-21s %s\n", "-w [text] [date]", "write new line to file with optional parameter [date] in format dd-mm-yyyy, which writes date to second column");
    printf("   %-21s %s\n", "-d [number]", "delete line [number]");
    printf("   %-21s %s\n", "-e [number]", "TODO"/*"print editable text from line [number] and save it edited back on line"*/);
    printf("   %-21s %s\n", "-s [number]", "sort lines 0 - in the order of writing, 1 - by date in second column, 2 - alphabetical");
    printf("   %-21s %s\n\n", "-o [option] [value]", "0 arguments - list options of noteedit, 1 argmunet - list value of option, 2 arguments - overwrite value of option.");
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
const int stprm = 6;//number of parameters in settings
const int nameLength = 20;//max length of parameters in settings 19+'\0'
const int valueLen = 4;//max digits of values of prameters 3+'\0'
const char adrNotes[] = "~/noteeditnt.txt";
const char adrSet[] = "~/noteeditst.txt";
struct Value{
    short sort;
    short dateFormat;
    short limitedLength;
    short limitedRows;
    int lineLength;
    int rows;
};
struct Alias{
    char sort[nameLength];
    char dateFormat[nameLength];
    char limitedLength[nameLength];
    char limitedRows[nameLength];
    char lineLength[nameLength];
    char rows[nameLength];
};
struct Settings{
    struct Value value;
    struct Alias alias;
};
struct Settings settings = {
    .value.sort = 1,
    .value.dateFormat = 0,
    .value.limitedLength = 1,
    .value.limitedRows = 1,
    .value.lineLength = 60,//allocation for arrays must be set +1 for '\0'
    .value.rows = 50,
    .alias.sort = "sort_by",
    .alias.dateFormat = "date_format",
    .alias.limitedLength = "Length_limitation",
    .alias.limitedRows = "row_limitation",
    .alias.lineLength = "line_length",
    .alias.rows = "rows"
};
char ** date_formates = {
    "00-00-0000",
    "00.00.0000"
};
struct ProcessInformation{
    int countLinesNotes;
    int datesLen;
    char ** text;
    char ** dates;
};
struct ProcessInformation procInfo;

int load_settings(FILE *ptr){
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
void write_settings(FILE *ptr){
    fprintf(ptr, "%s %s\n", settings.alias.sort, settings.value.sort);
    fprintf(ptr, "%s %s\n",settings.alias.dateFormat, settings.value.dateFormat);
    fprintf(ptr, "%s %s\n",settings.alias.lineLength, settings.value.lineLength);
    fprintf(ptr, "%s %s\n",settings.alias.rows, settings.value.rows);
}
void sort(){

}
int load_data(){
    FILE * fptr = fopen(adrNotes, "r");

    char chr;
    short space = 0;
    int ichr = 0;
    int index_line = 0;
    int part = 0;//1 after "X) " 2 after "|"
    while(fscanf(fptr, "%c", &chr)!=EOF){
        if (ichr == settings.value.lineLength) {printf("Too long text on line %d in %s.\nCan not be loaded into memory. %s or text file was modified.", index_line+1, adrNotes, settings.alias.lineLength); return 100;}
        switch (part){
            case 0: 
                if (chr == ' ') part = 1; break;
            case 1:
                if (chr == '|') 
                    {procInfo.text[index_line][ichr] = '\0'; part = 2;}
                else if (chr == '\n'){
                    part = 0;
                    procInfo.text[index_line][ichr] = '\0';
                    procInfo.dates[index_line][0] = '\0';
                    ichr = 0;
                    index_line++;
                }
                else{
                    if (chr != ' ' && space == 1) {
                        procInfo.text[index_line][ichr++] = ' ';
                        if (ichr == settings.value.lineLength) {printf("Too long text on line %d in %s.\nCan not be loaded into memory. %s or text file was manualy modified.", index_line+1, adrNotes, settings.alias.lineLength); return 100;}
                        procInfo.text[index_line][ichr++] = chr;
                        space = 0;
                    }else if (chr != ' ' && space == 0)
                        procInfo.text[index_line][ichr++] = chr;
                    else space = 1;
                }
                break;
            case 2:
                if (chr!='\n') procInfo.dates[index_line][ichr++] = chr;
                else {
                    if (strlen(procInfo.dates[index_line]) != procInfo.datesLen) {printf("Unexpected date format on line %d in %s.\nCan not be loaded into memory. %s or text file was manualy modified.", index_line+1, adrNotes, settings.alias.lineLength); return 101;}

                    ichr=0; 
                    index_line++; 
                    part = 0;
                }
                break;
        }
    }
}

typedef struct {
    char * text;
    char * date;
} Line;

/*
return text and date in one structure
@param num number of line which you want to get
@return pointer on structure
*/
Line * getLine(int index){
    static Line line;
    //allocation of memory for text + '\0'
    line.text = (char*)malloc((strlen(procInfo.text[index])+1)*sizeof(char));
    //mark down text (addres)
    line.text = procInfo.text[index];
    //uselles to allocate if NULL
    if (procInfo.dates[index]!=NULL)
        //allocation of memory for date + '\0'
        line.date = (char*)malloc((strlen(procInfo.dates[index])+1)*sizeof(char));
    //mark down date (addres) - NULL also wanted
    line.date = procInfo.dates[index];
    return &line;
}

enum Action{
    WRITE,EDIT,DEL
};

void formated_fprintf(FILE * fptr, int total_prefix_len, int line_index){
    //spaces before number of line
    int paddingStart = total_prefix_len - getRank(line_index);
    //padding of date to right
    int paddingDate = settings.value.lineLength - total_prefix_len - strlen((*procInfo.dates[line_index]))-3;//for spaces and ')'
    fprintf(fptr, "% *d) % *s|%d\n", paddingStart, line_index, paddingDate, (*procInfo.text[line_index]), (*procInfo.dates[line_index]));
}

/*
overwrite text file by lines in memory
@param fptr pointer into file moved on place where you want to start writing
@param lineNum number of line in memory with which begins writting
@param action is changing length of prefix depending on use of function - WRITE, DEL, EDIT
*/
void overwrite(FILE * fptr, int start_index, int end_index, enum Action act){
    //countLinesNotes is indexed from 0 -> +1, write -> count - 0 + 1, edit stays same -1 + 1, delete = -1
    int prefix = getRank(procInfo.countLinesNotes-act+1);

    for (int i=start_index; i<end_index; i++){
        formated_fprintf(fptr, prefix, i);
    }
}

/*
Delete line form text note
@param lineNum number of line which you want to delete
@param return_bool if it is not 0 then it gives back pointer on deleted line - for editing
@return pointer on string if exists; else NULL
*/
Line * delete(char* lineNum, short return_bool){
    int line_index = atoi(lineNum)-1;
    int tmp = load_notes();
    if (tmp!=0) return NULL;

    if (line_index<0 || line_index>=procInfo.countLinesNotes){
        printf("Invalid number of line.\n");
        return NULL;
    }
    else {
        //prepsani=smazani
        FILE * fptr = fopen(adrNotes, "r+");
        //write line like first - overwrite all
        if (line_index!=0){
            overwrite(fptr, 0, line_index, DEL);
        }
        //+1 skips line which we want to delete
        overwrite(fptr, line_index+1, procInfo.countLinesNotes, DEL);
        fclose(fptr);

        if (return_bool) return getLine(line_index);
        return NULL;
    }
}

/*
Check if writen text+date is in limits by length after adding another line
@param text is pointer on text which you want to write
@param date is pointer on date wich you want to write
@return 0 for no problem, 201 for too long text+date
*/
int checkLength(char * text, char * date){
    //length of prefix X)
    int prefix = getRank(procInfo.countLinesNotes+1)+2;//") " -> +2 ======== POZOR PRI VYUZITI V EDIT FUNKCI - POCET RADKU + 1 =========
    //only text
    if (date==NULL){
        //checking validity of length
        if (prefix + text >= settings.value.limitedLength){
            printf("Too long note.\nFor adding longer notes change or disable limit of lenght.\n");
            return 201;
        }
    }
    //text + date
    else{
        //checking validity of length
        if (prefix + strlen(*text) + strlen(*date) +1/*space before date*/>= settings.value.limitedLength){
            printf("Too long note.\nFor adding longer notes change or disable limit of lenght.\n"); 
            return 201;
        }
    }
    return 0;
}

/*
@return length of given number by logarithm
*/
int getRank(int number){
    return (int)floor(log10(number));
}

/*
Compares dates in format of this script
@param date pointer on date which should be written down
@return integer of position/line for writing down
*/
int cmpDates(char* date){
    //nothing to compare - write it at the end
    if (date == NULL) return -1;

    int input = strlen(date);
    for (int i = 0; i<procInfo.countLinesNotes; i++){
        int cmp = strlen(procInfo.dates[i]);
        //doesnt have a date
        if (cmp==0) return i;

        //get year - SETUPED FOR 4 CHARACTERS FORMAT
        int yearForm = 4;
        char yearDate[5] = {date[input-4], date[input-3], date[input-2], date[input-1], '\0'};
        char yearCmp[5] = {procInfo.dates[i][cmp-4], procInfo.dates[i][cmp-3], procInfo.dates[i][cmp-2], procInfo.dates[i][cmp-1], '\0'};

        //lower year -> return index to write here
        //higher year -> compare with another line
        int tmpD = atoi(yearDate);
        int tmpC = atoi(yearCmp);
        if (tmpD < tmpC) return i;
        else if (tmpD > tmpC) continue;

        //get month
        //-yyyy -> yearForm+2
        char monthDate[3] = {date[input-yearForm-3], date[input-yearForm-2], '\0'};
        char monthCmp[3] = {procInfo.dates[i][cmp-yearForm-3], procInfo.dates[i][cmp-yearForm-2], '\0'};
        
        //lower month -> return index to write here
        //higher month -> compare with another line
        //atoi("-2")==2 -> doesnt matter if number is of lenght 1
        int tmpD = atoi(monthDate);
        int tmpC = atoi(monthCmp);
        if (tmpD < tmpC) return i;//indexing from 1
        else if (tmpD > tmpC) continue;

        //get day
        //now from the begining 
        char dayDate[3] = {date[0], date[0], '\0'};
        char dayCmp[3] = {procInfo.dates[i][0], procInfo.dates[i][0], '\0'};
        
        //lower month -> return index to write here
        //higher month -> compare with another line
        //atoi("2-")==2 -> doesnt matter if number is of lenght 1
        int tmpD = atoi(dayDate);
        int tmpC = atoi(dayCmp);
        if (tmpD > tmpC) continue;

        //equal or earlier
        //later added date goes on top
        return i;
    }
    //later than everything or date wasnt added
    return -1;
}
/*
Compare text by strcmp()
@param text pointer on text to compare
@return index where to put text to by alphabetical
*/
int cmpAlph(char * text){
    for (int i = 0; i<procInfo.countLinesNotes; i++){
        int cmpOutput = strcmp(text, procInfo.text[i]);
        if (cmpOutput >= 0) return i;
    }
    return -1; //append at the end
}

/*
Insert line to text file in correct order
@param text text of note
@param date date connected to note
@return 0 as succesful run
*/
int write(char * text, char * date){
    //0 = runs successfully
    int tmp = load_settings();
    if (tmp!=0) return tmp;
    tmp = load_notes();
    if (tmp!=0) return tmp;

    //limit of lines is enabled and reached
    if (settings.value.limitedRows && procInfo.countLinesNotes==settings.value.rows) 
        printf("Too many lines in notes.\nFor adding more lines change or disable limit of lines.\n"); return 200;

    //limit of length of line is enabled
    if (settings.value.limitedLength){
        int tmp = checkLength(text, date);
        if (tmp!=0) return tmp;
    }
    //check type of sort
    int position;
    switch (settings.value.sort){
        case 0: position = -1; break;
        case 1: position = cmpDates(date); break;
        case 2: position = cmpAlph(text); break;
    }
    //get pointer to file from index of line
    FILE * fptr = fopen(adrNotes, "r+");

    if (position!=-1){
        overwrite(fptr, 0, position, WRITE);

        int total_prefix_len = getRank(procInfo.countLinesNotes);
        //spaces before number of line
        int paddingStart = total_prefix_len - getRank(position);
        //padding of date to right
        int paddingDate = settings.value.lineLength - total_prefix_len - strlen((*date))-3;//for spaces and ')'
        fprintf(fptr, "% *d) % *s|%d\n", paddingStart, position, paddingDate, (*text), (*date));

        overwrite(fptr, position, procInfo.countLinesNotes,WRITE);
    }else{
        overwrite(fptr, 0, procInfo.countLinesNotes, WRITE);

        int total_prefix_len = getRank(procInfo.countLinesNotes);
        //spaces before number of line
        int paddingStart = total_prefix_len - getRank(position);
        //padding of date to right
        int paddingDate = settings.value.lineLength - total_prefix_len - strlen((*date))-3;//for spaces and ')'
        fprintf(fptr, "% *d) % *s|%d\n", paddingStart, position, paddingDate, (*text), (*date));
    }
    fclose(fptr);
    return 0;
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
1) text1                     |01-01-2025
2) text2                     |02-03-2025
3) text3                     |10-06-2025
*/

int main(int argc, char *arg[]){
    char arguments[] = {
        "-h", "-d", "-w",
        "-e", "-s", "-o",//options
        "-v"
    };
    if (argc>1){
        for (int i=1; i<argc; i++){//ignore argument with path of script
            //separate arguments and check its validity
            char ** input = parse_and_check(argc ,arg);
            //returns if input is invalid
            if (input==NULL) return 100;
            else{
                switch(input[0][0]){
                    //help -h
                    case 0: helpGuide();break;
                    //version -v
                    case 1: printf("%s\n",fileInformation.version);break;
                    //write with one argument -w "arg"
                    case 2: write(input[1], NULL);break;
                    //write with two arguments -w "arg" "date"
                    case 3: write(input[1],input[2]);break;
                    //delete -d "number"
                    case 4: delete(input[1], 0);break;
                    //edit -e "number"
                    case 5: edit(input[1]);break;
                    //sort -s "0/1/2"
                    case 6: sort(input[1]);break;
                    //options list -o
                    case 7: listSettings(NULL);break;
                    //options list value of -o "option"
                    case 8: listSettings(input[1]);break;
                    //change option -o "option" "value"
                    case 9: changeOption();break;
                    default: printf("How did you get there? Report thist bug as default argument on github: xy\n");
                }
            }
        }
    }
    else printf("missing arguments (type -h for help)\n");

    return 0;
}
