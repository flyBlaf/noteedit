#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h> //compile with -lm at the end

//return value 
/*
0 - program runs succesfully
1 - terminated by user
100 - saved lines in notes are too long to load into memory
101 - length of dates doesnt match with example
102 - too much keywords in setting file
103 - limit of rows is reached
104 - too long line
200 - malloc returns NULL
*/

//--------------------
//TODO list
//--------------------
/*
    zkontrolovat indexovani od 0
    odsazovani ve funkcich overwrite a write
    funkce edit, sort
    date formates
    limitace znaku a radku
*/

//default settings are overwritten from noteeditset.txt
//sort - 0/NONE - in order of writing, 1/DATE - by date, 2/ALPHA - alphabetical | default value 1/DATE
//dateFormat - TODO
//line length - limit for length of text to fit in conky window
//rows - limit for number of rows
#define stprm 6//number of parameters in settings
#define nameLength 20//max length of parameters in settings 19+'\0'
#define valueLenght 8 //max lenght of value of keyword

char adrNotes[256];
char adrSet[256];
char *name_notes = "/noteedit_notes.txt";
char *name_settings = "/noteedit_set.txt";
int allocations = -1;
short allocBoth = 1;

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

//======================= Constants and file information ========================================================

/*
None - sorted in order of writing
Date
Alphabeticaly
*/
enum sort_type{
    NONE, DATE, ALPHA
};

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
    .value.sort = DATE,
    .value.dateFormat = 0,
    .value.limitedLength = 1,
    .value.limitedRows = 1,
    .value.lineLength = 60,//allocation for arrays must be set +1 for '\0'
    .value.rows = 50,
    .alias.sort = "sort_by",
    .alias.dateFormat = "date_format",
    .alias.limitedLength = "length_limitation",
    .alias.limitedRows = "row_limitation",
    .alias.lineLength = "line_length",
    .alias.rows = "rows"
};

struct ProcessInformation{
    int countLinesNotes;
    char ** text;
    char ** dates;
};
struct ProcessInformation procInfo;

char * date_formates[] = {
    "00-00-0000",
    "00.00.0000"
};

//================================== Aditable variables by script and default values =============================================

void helpGuide(){
    for (int i=0; i<60; i++){printf("=");}
    printf("\nThis is help guide for %s program\n", fileInformation.name);
    for (int i=0; i<60; i++){printf("=");}
    printf("\n   Author: %s\n", fileInformation.author);
    printf("   Version: %s\n", fileInformation.version);
    printf("   Written in %s\n", fileInformation.year);
    for (int i=0; i<30; i++){printf("-");}
    printf("\n\nThis program is formating lines of text into text file.\nText is divided into two columns - 1.subject 2.Date and by default it is sort by date.\nLines without date are in the order of writing");
    printf("\n\n   %-21s %s\n", "-h", "print (this) help guide");
    printf("   %-21s %s\n", "-v", "print version");
    printf("   %-21s %s\n", "-w [text] [date]", "write new line to file with optional parameter [date] in format dd-mm-yyyy, which writes date to second column");
    printf("   %-21s %s\n", "-d [number]", "delete line [number]");
    printf("   %-21s %s\n", "-e [number]", "TODO"/*"print editable text from line [number] and save it edited back on line"*/);
    printf("   %-21s %s\n\n", "-o [option] [value]", "0 arguments - list options of noteedit, 1 argmunet - list value of option, 2 arguments - overwrite value of option.");
    printf("For changes and new versions (if a new version ever comes) look for TODO-github.\n");
}

int errorQuestion(){
    printf("Some data can be lost. Do you want to continue? [Y/n]");
    char c;
    scanf(" %c\n",&c);
    if (c=='Y') return 0;
    else{
        printf("Proccess canceled...\n");
        return 1;
    }
    return 0;
}

void listSettings(char * alias){
    if (alias == NULL) goto end;

    if (strcmp(alias, settings.alias.lineLength)==0)            printf("   %d %-21s - maximal number of characters on single line.\n", settings.value.lineLength, settings.alias.lineLength);
    else if (strcmp(alias, settings.alias.rows)==0)             printf("   %d %-21s - maximal number of rows.\n", settings.value.rows, settings.alias.rows);
    else if (strcmp(alias, settings.alias.dateFormat)==0)       printf("   %d %-21s - actual used date format.\n", settings.value.dateFormat, settings.alias.dateFormat);
    else if (strcmp(alias, settings.alias.limitedLength)==0)    printf("   %d %-21s - number of characters is limited 0=False/1=True.\n", settings.value.limitedLength, settings.alias.limitedLength);
    else if (strcmp(alias, settings.alias.limitedRows)==0)      printf("   %d %-21s - number of rows is limited 0=False/1=True.\n", settings.value.limitedRows, settings.alias.limitedRows);
    else if (strcmp(alias, settings.alias.sort)==0)             printf("   %d %-21s - chose of sorting parameter 0 - none/in order of writing, 1 - by date, 2 - by alphabet\n", settings.value.sort, settings.alias.sort);
    else{
        end:
        printf("   %d %-21s - maximal number of characters on single line.\n", settings.value.lineLength, settings.alias.lineLength);
        printf("   %d %-21s - maximal number of rows.\n", settings.value.rows, settings.alias.rows);
        printf("   %d %-21s - actual used date format.\n", settings.value.dateFormat, settings.alias.dateFormat);
        printf("   %d %-21s - number of characters is limited 0=False/1=True.\n", settings.value.limitedLength, settings.alias.limitedLength);
        printf("   %d %-21s - number of rows is limited 0=False/1=True.\n", settings.value.limitedRows, settings.alias.limitedRows);
        printf("   %d %-21s - chose of sorting parameter 0 - none/in order of writing, 1 - by date, 2 - by alphabet\n", settings.value.sort, settings.alias.sort);
    }
}

//=================================== Informative Functions =================================================

/*
@return length of given number + 1 by logarithm
*/
int getRank(int number){
    return (int)floor(log10(number+1))+1;
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
    line.text = procInfo.text[index];
    line.date = procInfo.dates[index];
    return &line;
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
        if (prefix + strlen(text) >= settings.value.lineLength){
            printf("Too long note.\nFor adding longer notes change or disable limit of lenght.\n");
            return 104;
        }
    }
    //text + date
    else{
        //checking validity of length
        if (prefix + strlen(text) + strlen(date) +1/*space before date*/>= settings.value.lineLength){
            printf("Too long note.\nFor adding longer notes change or disable limit of lenght.\n"); 
            return 104;
        }
    }
    return 0;
}

enum Action{
    WRITE,EDIT,DEL
};

void formated_fprintf(FILE * fptr, int total_prefix_len, int line_index, int number){
    //spaces before number of line
    int paddingStart = total_prefix_len - getRank(line_index);
    //with and without date
    if (procInfo.dates[line_index]==NULL) fprintf(fptr, "%*d) %s\n", paddingStart, number, procInfo.text[line_index]);
    else{
        //padding of date to right
        int paddingDate = settings.value.lineLength - total_prefix_len - strlen(procInfo.dates[line_index])-3;//for spaces and ')'
        fprintf(fptr, "%*d) %-*s|%s\n", paddingStart, number, paddingDate, procInfo.text[line_index], procInfo.dates[line_index]);
    }
}

/*
overwrite text file by lines in memory
@param fptr pointer into file moved on place where you want to start writing
@param lineNum number of line in memory with which begins writting
@param action is changing length of prefix depending on use of function - WRITE, DEL, EDIT
@param addition is awful way how to change printed number on start of line when it is not corespondent with index+1
*/
void overwrite(FILE * fptr, int start_index, int end_index, enum Action act, int additon){
    //countLinesNotes is indexed from 0 -> +1, write -> count - 0 + 1, edit stays same -1 + 1, delete = -1
    int prefix = getRank(procInfo.countLinesNotes-act+1+additon);

    for (int i=start_index; i<end_index; i++){
        formated_fprintf(fptr, prefix, i, i+additon);
    }
}

/*
Compares dates in format of this script
@param date pointer on date which should be written down
@return index of where to write down new text; -1 as last one
*/
int cmpDates(char* date){
    //nothing to compare - write it at the end
    if (date == NULL) return -1;

    int input = strlen(date);
    for (int i = 0; i<procInfo.countLinesNotes; i++){
        //doesnt have a date
        if (procInfo.dates[i]==NULL) return i;
        int cmp = strlen(procInfo.dates[i]);

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
        tmpD = atoi(monthDate);
        tmpC = atoi(monthCmp);
        if (tmpD < tmpC) return i;//indexing from 1
        else if (tmpD > tmpC) continue;

        //get day
        //now from the begining 
        char dayDate[3] = {date[0], date[1], '\0'};
        char dayCmp[3] = {procInfo.dates[i][0], procInfo.dates[i][1], '\0'};
        
        //lower month -> return index to write here
        //higher month -> compare with another line
        //atoi("2-")==2 -> doesnt matter if number is of lenght 1
        tmpD = atoi(dayDate);
        tmpC = atoi(dayCmp);
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
        if (cmpOutput < 0) return i;
    }
    return -1; //append at the end
}

int change_date_format(){
    return 0;
}

//===================================== simplifing functions ==========================================

int load_settings(){
    FILE * fptr = fopen(adrSet, "r");
    if (fptr==NULL) return 0;

    char c;
    char keyword[stprm][nameLength];//19 characters of name
    int value[stprm] = {-1};
    char str_value[valueLenght];
    int load_part = 0;//0-keyword, 1-value
    int invalidChracter = 0;
    int index_word=0;
    int ic=0;
    while(fscanf(fptr, "%c", &c)!=EOF){
        if (c==' '&&ic==0) continue;

        if (load_part==0){//load keyword
            if (c=='\n'&&ic==0) continue;

            if (index_word==stprm) {printf("Too much keywords in settings.\nCheck %s or increase stprm in script", adrNotes);return 102;}

            if (ic==nameLength-1) {//space for '\0'
                printf("Too long keyword.\nCheck %s or increase nameLength(=%d) otherwise it will be discard and replaced by missing default keyword and value.\n", adrSet, nameLength-1);

                int status = 0;
                while(status==EOF || c=='\n') //discard line
                    status = fscanf(fptr, "%c", &c);
                continue;
            }
            else if (c==' '){//prepare for value
                keyword[index_word][ic] = '\0';
                load_part = 1;
                ic = 0;
            }
            else if (c=='\n'){//missing value
                keyword[index_word][ic] = '\0';
                printf("Missing value after keyword %s.\nIt will be replaced by default value.\n", keyword[index_word++]);

                ic = 0;
            }
            else keyword[index_word][ic++] = c;

        }else{
            if (ic==valueLenght-1) {//space for '\0'
                printf("Too high value after keyword %s.\nCheck %s or increase nameLength(=%d) otherwise it will be replaced by default value.\n", keyword[index_word++], adrSet, valueLenght-1);

                int status = 0;
                while(status!=EOF || c!='\n') //discard line
                    status = fscanf(fptr, "%c", &c);
                continue;
            }
            else if (c=='\n' && ic == 0){
                printf("Missing value after keyword %s.\nIt will be replaced by default value.\n", keyword[index_word++]);

                load_part = 0;
                ic = 0;
            }else if (c=='\n'){
                str_value[ic]='\0';
                value[index_word++] = atoi(str_value);
                load_part = 0;
                ic = 0;
            }
            else str_value[ic++] = c;
        }
    }
    fclose(fptr);

    for (int i = 0; i < index_word; i++){
        short warning = 0;
        //date format
        if (strcmp(keyword[i], settings.alias.dateFormat)==0){
            if (value[i]>=0 && value[i]<(sizeof(date_formates)/sizeof(date_formates[0])))
                settings.value.dateFormat = value[i];
            else warning = 1;
        }
        //limit of length (true/false)
        else if (strcmp(keyword[i], settings.alias.limitedLength)==0){
            if (value[i]==0 || value[i]==1) settings.value.limitedLength = value[i];
            else warning = 1;
        }
        //limit of rows (true/false)
        else if (strcmp(keyword[i], settings.alias.limitedRows)==0){
            if (value[i]==0 || value[i]==1) settings.value.limitedRows = value[i];
            else warning = 1;
        }
        //max length of line
        else if (strcmp(keyword[i], settings.alias.lineLength)==0){
            if (value[i]>0) settings.value.lineLength = value[i];
            else warning = 1;
        }
        //max number of rows        
        else if (strcmp(keyword[i], settings.alias.rows)==0){
            if (value[i]>0) settings.value.rows = value[i];
            else warning = 1;
        }
        //type of sort
        else if (strcmp(keyword[i], settings.alias.sort)==0){
            if (value[i]>=0 && value[i]<=2) settings.value.sort = value[i];
            else warning = 1;
        }
        else {
            printf("Unknown keyword %s. It will be discard.", keyword[i]);
            if (errorQuestion()==1) return 1;
        }
        if (warning==1){
            printf("Unexpected value of %s. It will be set on default value.\n", keyword[i]);
            if (errorQuestion()==1) return 1;
        }
    }
    return 0;
}

void write_settings(){
    FILE * fptr = fopen(adrSet, "w");
    fprintf(fptr, "%s %d\n", settings.alias.sort, settings.value.sort);
    fprintf(fptr, "%s %d\n",settings.alias.dateFormat, settings.value.dateFormat);
    fprintf(fptr, "%s %d\n",settings.alias.lineLength, settings.value.lineLength);
    fprintf(fptr, "%s %d\n",settings.alias.rows, settings.value.rows);
    fprintf(fptr, "%s %d\n",settings.alias.limitedLength, settings.value.limitedLength);
    fprintf(fptr, "%s %d\n",settings.alias.limitedRows, settings.value.limitedRows);
    fclose(fptr);
}

int load_data(){
    FILE * fptr = fopen(adrNotes, "r");
    if (fptr == NULL) return 0;

    //allocation of memory for text and dates
    allocations = -1; //for freeing in case of return
    allocBoth = 1;
    char ** tmpText = (char**)malloc(settings.value.rows * sizeof(char*));
    if (tmpText==NULL) {printf("Allocation failed. Not enough memory.\n"); return 200;}
    char ** tmpDate = (char**)malloc(settings.value.rows * sizeof(char*));
    if (tmpDate==NULL) {printf("Allocation failed. Not enough memory.\n"); allocBoth=0; return 200;}
    allocations++;

    for (; allocations < settings.value.rows; allocations++){
        tmpText[allocations] = (char*)malloc((settings.value.lineLength+1) * sizeof(char));
        if (tmpText[allocations]==NULL) {printf("Allocation failed. Not enough memory.\n"); return 200;}
        tmpDate[allocations] = (char*)malloc((strlen(date_formates[settings.value.dateFormat])+1) * sizeof(char));
        if (tmpDate[allocations]==NULL) {printf("Allocation failed. Not enough memory.\n"); allocBoth=0; return 200;}
    }
    allocations--;//to be same as in return cases
    procInfo.text = tmpText;
    procInfo.dates = tmpDate;

    char chr;
    short space = 0;
    int ichr = 0;
    int index_line = 0;
    int part = 0;//1 after "X) " 2 after "|"
    while(fscanf(fptr, "%c", &chr)!=EOF){
        if (ichr == settings.value.lineLength) {printf("Too long text on line %d in %s.\nCan not be loaded into memory. %s or text file was modified.", index_line+1, adrNotes, settings.alias.lineLength); return 100;}
        switch (part){
            case 0: 
                if (chr == ')') {part = 1; fscanf(fptr, "%c", &chr);} break;//read space and ignore - does not checking case -w ""
            case 1:
                if (chr == '|') {
                    procInfo.text[index_line][ichr] = '\0'; 
                    ichr = 0;
                    part = 2;
                }
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
                    procInfo.dates[index_line][ichr] = '\0';

                    if (strlen(procInfo.dates[index_line]) != strlen(date_formates[settings.value.dateFormat])) {printf("Unexpected date format on line %d in %s.\nCan not be loaded into memory. %s or text file was manualy modified.\n", index_line+1, adrNotes, settings.alias.lineLength); return 101;}

                    ichr=0; 
                    index_line++; 
                    part = 0;
                }
                break;
        }
    }
    procInfo.countLinesNotes = index_line;
    fclose(fptr);

    return 0;
}

//==================================== work with memory =====================================

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
    tmp = load_data();
    if (tmp!=0) return tmp;

    //limit of lines is enabled and reached
    if (settings.value.limitedRows && procInfo.countLinesNotes==settings.value.rows) {
        printf("Too many lines in notes.\nFor adding more lines change or disable limit of lines.\n"); return 103;
    }

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

    FILE * fptr = fopen(adrNotes, "w");

    if (position!=-1){
        overwrite(fptr, 0, position, WRITE, 0);

        //increase from index
        int number = position+1;

        int total_prefix_len = getRank(procInfo.countLinesNotes+1);
        //spaces before number of line
        int paddingStart = total_prefix_len - getRank(position);

        //with and without date
        if (date == NULL) fprintf(fptr, "%*d) %s\n", paddingStart, number, text);
        else{
            //padding of date to right
            int paddingDate = settings.value.lineLength - total_prefix_len - strlen(date)-3;//for spaces and ')'
            fprintf(fptr, "%*d) %-*s|%s\n", paddingStart, number, paddingDate, text, date);
        }

        overwrite(fptr, position, procInfo.countLinesNotes,WRITE, 1);
    }else{
        overwrite(fptr, 0, procInfo.countLinesNotes, WRITE, 0);
        int total_prefix_len = getRank(procInfo.countLinesNotes+1);
        //with and without date
        if (date == NULL) fprintf(fptr, "%d) %s\n", procInfo.countLinesNotes+1, text);
        else{
            //padding of date to right
            int paddingDate = settings.value.lineLength - total_prefix_len - strlen(date)-3;//for spaces and ')'
            fprintf(fptr, "%d) %-*s|%s\n", procInfo.countLinesNotes+1, paddingDate, text, date);
        }
    }
    fclose(fptr);
    return 0;
}

/*
Delete line form text note
@param lineNum number of line which you want to delete
@param return_bool if it is not 0 then it gives back pointer on deleted line - for editing
@return pointer on string if exists; else NULL
*/
Line * delete(char* lineNum){
    int line_index = atoi(lineNum)-1;
    int tmp = load_data();
    if (tmp!=0) return NULL;

    if (line_index<0 || line_index>=procInfo.countLinesNotes){
        printf("Invalid number of line.\n");
        return NULL;
    }
    else {
        //prepsani=smazani
        FILE * fptr = fopen(adrNotes, "w");
        //write line like first - overwrite all
        if (line_index!=0){
            overwrite(fptr, 0, line_index, DEL, 0);
        }
        //+1 skips line which we want to delete
        overwrite(fptr, line_index+1, procInfo.countLinesNotes, DEL, -1);
        fclose(fptr);

        return getLine(line_index);
    }
}

int Edit(char * lineNum){
    return 0;
}

int Sort(){
    switch (settings.value.sort){
        case NONE:
        case DATE:
        case ALPHA:
        default:
            printf("Unexpected value.\nFor sort are only options 0/1/2.\n");
    }
    return 0;
}

int Options(char * option, char * value){
    int tmp = load_settings();
    if (tmp!=0) return tmp;

    short warning = 0;
    int intValue;
    if (value!=NULL){
        intValue = atoi(value);
        if (intValue == 0 && value[0]!='0') intValue = -1;
    }

    if (option == NULL) listSettings(NULL);
    else if (value == NULL) listSettings(option);

    //change settings
    //date format
    else if (strcmp(option, settings.alias.dateFormat)==0){
        if (intValue>=0 && intValue<(sizeof(date_formates)/sizeof(date_formates[0]))){
            settings.value.dateFormat = intValue; change_date_format();
        }else warning = 1;
        }
    //limit of length (true/false)
    else if (strcmp(option, settings.alias.limitedLength)==0){
        if (intValue==0 || intValue==1) settings.value.limitedLength = intValue;
        else warning = 1;
    }
    //limit of rows (true/false)
    else if (strcmp(option, settings.alias.limitedRows)==0){
        if (intValue==0 || intValue==1) settings.value.limitedRows = intValue;
        else warning = 1;
    }
    //max length of line
    else if (strcmp(option, settings.alias.lineLength)==0){
        if (intValue>0) settings.value.lineLength = intValue;
        else warning = 1;
    }
    //max number of rows        
    else if (strcmp(option, settings.alias.rows)==0){
        if (intValue>0) settings.value.rows = intValue;
        else warning = 1;
    }
    //type of sort
    else if (strcmp(option, settings.alias.sort)==0){
        if (intValue>=0 && intValue<=2) {settings.value.sort = intValue; Sort();}
        else warning = 1;
    }
    else printf("Option %s does not exist in settings.\n", option);

    if (warning == 1) printf("Unexpected value.\nType \"-o\" for list settings or \"-h\" for help.\n");

    write_settings();

    return 0;
}

// ===================================== Aplication actions ======================================================

//noteeditset.txt - saved settings
/*
sortBy 1
dateFormat 1
lineLength 60
rows 50
...
*/
//notes.txt - this is printed
/*
1) text1                     |01-01-2025
2) text2                     |02-03-2025
3) text3                     |10-06-2025
*/

int main(int argc, char *arg[]){
    //find directory
    //adrNotes = malloc(strlen(getenv("HOME") + strlen(name_notes) + 1)); // to account for NULL terminator
    strcpy(adrNotes, getenv("HOME"));
    strcat(adrNotes, name_notes);
    //adrSet = malloc(strlen(getenv("HOME") + strlen(name_settings) + 1)); // to account for NULL terminator
    strcpy(adrSet, getenv("HOME"));
    strcat(adrSet, name_settings);

    int rtn_value = 0;
    if (argc>1){
        if (arg[1][0] == '-')
            switch(arg[1][1]){
                //help -h
                case 'h': helpGuide();break;
                //version -v
                case 'v': printf("version %s\n",fileInformation.version);break;
                //write with one argument -w "arg" "date"
                case 'w': rtn_value = write(arg[2], (argc>3) ? arg[3] : NULL);break;
                //delete -d "number"
                case 'd': delete(arg[2]);break;
                //edit -e "number"
                case 'e': rtn_value = Edit(arg[2]);break;
                //options list -o "option" "value"
                case 'o': rtn_value = Options((argc>2)? arg[2]:NULL, (argc>3)? arg[3]:NULL);break;
                default: printf("Invalid arguments (type -h for help).\n");
            }
        else printf("Invalid arguments (type -h for help).\n");
    }
    else printf("Missing arguments (type -h for help).\n");

    //free memory
    if (procInfo.text!=NULL){
        for (int i = 0; i <= allocations; i++){
            free(procInfo.text[i]);
            if (allocBoth)
                free(procInfo.dates[i]);
        }
        free(procInfo.text);
        if (allocBoth && allocations==-1)
            free(procInfo.dates);
    }

    return rtn_value;
}
