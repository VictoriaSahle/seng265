/* File: csvfunctions.h

   DO NOT CHANGE THIS FILE!!
*/

#define MAXROWS     20
#define MAXCOLS     12
#define MAXLEN      16

struct SpreadSheet {
    char *fileName;
    int  numRows;
    int  numCols;
    char contents[MAXROWS][MAXCOLS][MAXLEN];
};

typedef struct SpreadSheet SPREADSHEET;

// prototypes for the module functions
extern SPREADSHEET *SS_ReadCSV(char *fileName);
extern void SS_SetDebug(int debug);
extern int SS_FindRow(SPREADSHEET *ss, int colNum, char *text, int startNum);
extern void SS_PrintRow(SPREADSHEET *ss, int rowNum);
extern double SS_ColumnSum(SPREADSHEET *ss, int colNum);
extern double SS_ColumnAvg(SPREADSHEET *ss, int colNum);

