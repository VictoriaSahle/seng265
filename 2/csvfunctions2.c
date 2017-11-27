/*
 * ********************************************
 * *** ID                  : xxxxxxxxx
 * *** Name                : Victoria Sahle
 * *** Date                : October 7, 2014
 * *** Program Name        : csvfunctions2.c
 * ********************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvfunctions2.h"

#define MAXINPUTLINELEN     256
#define MAXITEMSPERROW		128

#define CHECKMALLOC(p)	if ((p)==NULL) { fprintf(stderr,"out of memory!"); exit(1); } else { }

static int debug = 0;


// forward declarations
static int extractItems(char *line, char *row[]);
char *mystrdup(char *s);

void SS_SetDebug(int dbg) {
    debug = dbg;
}

SPREADSHEET *SS_ReadCSV(char *fileName) {
    char line[MAXINPUTLINELEN];
    char *tempRow[MAXITEMSPERROW];
    SPREADSHEET *result;
    struct OneRow *lastRow = NULL;
    int i;

	result = malloc(sizeof(SPREADSHEET));
	CHECKMALLOC(result);
    result->fileName = mystrdup(fileName);
    result->firstRow = NULL;
    result->numRows = result->numCols = 0;

    FILE *f = fopen(fileName, "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to read from file %s\n", fileName);
        perror(fileName);
        return NULL;
    }
    for( i = 0; ; i++) {
        if (fgets(line, MAXINPUTLINELEN, f) == NULL)
            break;
        int k = extractItems(line, tempRow);
        if (result->numCols == 0) {
            result->numCols = k;
        } else
        if (result->numCols != k) {
            fprintf(stderr, "Row %d has different number of columns from first row\n", i);
            continue;	// ignore this row
        }

        // instantiate the storage for the new row and copy k cells into it
        char **rc = calloc(k, sizeof(char *));
        CHECKMALLOC(rc);
        struct OneRow *newrow = malloc(sizeof(struct OneRow));
        CHECKMALLOC(newrow);
        newrow->row = rc;
        newrow->nextRow = NULL;
        int ix;
        for( ix = 0; ix < k; ix++ ) {
            rc[ix] = tempRow[ix];
        }
        
        // add the new row as the last row in the spreadsheet
        if (lastRow == NULL) {
            result->firstRow = newrow;
        } else {
            lastRow->nextRow = newrow;
        }
        lastRow = newrow;

    }
    result->numRows = i;
    fclose(f);
    return result;
}

// Write the spreadsheet in CSV format to the specified file 
void SS_SaveCSV(SPREADSHEET *ss, char *fileName) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_SaveCSV(--)\n");

    FILE *f;
    if (fileName == NULL)
   	 f = fopen(ss->fileName, "w");
    else
	f = fopen(fileName, "w");
    if (f == NULL) {
	if(fileName == NULL){
       	   fprintf(stderr, " %s\n", ss->fileName);
           perror(ss->fileName);
	}else{
	   fprintf(stderr, " %s\n", fileName);
	   perror(fileName);
	}
        return;
    }
    struct OneRow *rp = ss->firstRow;
    int k;
    const char *quote = "\"";
    for(k = 0; k < (ss->numRows) && rp != NULL; k++) {
        int i;
        for( i = 0 ; i < ss->numCols; i++ ) {
            if (i > 0)
                fputs(",", f);
            if (strchr(rp->row[i], '"') != NULL) {
                int j;
                fputs(quote, f);
                for (j = 0; j < strlen(rp->row[i]); j++){
                    if ((rp->row[i][j]) == '"')
                        fputc('"', f);
                    fputc(rp->row[i][j], f);
                }
                fputs(quote, f);
            } else {
                fputs(quote, f);
                fputs(rp->row[i], f);
                fputs(quote, f);
            }
        }
        fputs("\r\n", f);
        rp = rp->nextRow;
    }
    fclose(f);
	
}

// Free all storage being use by the spreadsheet instance.
extern void SS_Unload(SPREADSHEET *ss) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_Unload(--)\n");
    if(ss->numRows > 0){
        int i;
        for (i = 0; i < ss->numRows; i++)
            SS_DeleteRow(ss, i);
    }        
	free(ss);
}

// Reads one item from the csv row.
// line is a string where reading should begin; tok reference a char array into
// which the characters for the item are copied.
// On return, the result references the remainder of the original string which has
// not been read, or is NULL if no item could be read before the end of the line.
static char *getOneItem(char *line, char *tok) {
    char *tokSaved = tok;
    char c;
    c = *line++;
S1: if (c == '\"') {
        c = *line++;
        goto S2;
    }
    if (c == ',' || c == '\0' || c == '\n' || c == '\r') {
        goto S4;
    }
    *tok++ = c;
    c = *line++;
    goto S1;
S2: if (c == '\"') {
        c = *line++;
        goto S3;
    }
    if (c == '\0' || c == '\n' || c == '\r') {
        // unexpected end of input line
        fprintf(stderr, "mismatched doublequote found");
        goto S4;
    }
    *tok++ = c;
    c = *line++;
    goto S2;
S3: if (c == '\"') {
        *tok++ = '\"';
        c = *line++;
        goto S2;
    }
    if (c == ',' || c == '\0' || c == '\n' || c == '\r') {
        goto S4;
    }
    *tok++ = c;
    c = *line++;
    goto S1;
S4: if (c == '\0' || c == '\n' || c == '\r') {
        if (tokSaved == tok)
            return NULL;  // nothing was read
        line--;
    }
    *tok = '\0';
    return line;
}

// Extracts items one by one from line, copies them into heap storage,
// and stores references to them in the row array.
// The function result is the number of items copied.
static int extractItems(char *line, char *row[]) {
    char t[MAXINPUTLINELEN];
    int col = 0;
    for( ; ; ) {
        line = getOneItem(line,t);
        if (line == NULL) break;
        char *s = mystrdup(t);
        row[col++] = s;
        
    }
    return col;
}

// prints filename, number of rows and number of columns for
// this spreadsheet
void SS_PrintStats(SPREADSHEET *ss) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_PrintStats(--)\n");
    printf("File:  %s\n", ss->fileName);
    printf("Rows:  %d\n", ss->numRows);
    printf("Columns:  %d\n", ss->numCols);
}


// Transfers rows from spreadsheet ss2 to the end of spreadsheet ss1
// then releases any storage for ss2 which is no longer needed
void SS_MergeCSV(SPREADSHEET *ss1, SPREADSHEET *ss2) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_MergeCSV(--, --)\n");
if(ss1->numCols != ss2->numCols){
		printf("Number of columns do not match.\n");
		SS_Unload(ss2);
		return;

	}
	struct OneRow *rp = ss1->firstRow;
	struct OneRow *fr2 = ss2->firstRow;
	while(rp->nextRow != NULL){
		rp = rp->nextRow;
	}	
	
	while(fr2->nextRow != NULL){
		struct OneRow *newrow = malloc(sizeof(struct OneRow));
		rp->nextRow = newrow;
		rp = rp->nextRow;
		*newrow = *fr2;
		fr2 = fr2->nextRow;
		
	}
	struct OneRow *newrow = malloc(sizeof(struct OneRow));
	rp->nextRow = newrow;
	*newrow = *fr2;

	ss1->numRows = (ss1->numRows) + (ss2->numRows);
	SS_Unload(ss2); 
}

// Deletes the specified row from the spreadsheet.
// Any storage used by the row is freed.
void SS_DeleteRow(SPREADSHEET *ss, int rowNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_DeleteRow(--,%d)\n", rowNum);
   
    if (rowNum >= ss->numRows || rowNum < 0) {
        printf("Row number (%d) is out of range\n", rowNum);
        return;
    }
    struct OneRow *tempRow;
    struct OneRow *prevRow;
    struct OneRow *rp = ss->firstRow;
    struct OneRow *fr = ss->firstRow;
    if (rowNum == 0){
        tempRow = rp;
        rp = tempRow->nextRow;
        free(tempRow);
        ss->firstRow = rp;
        (ss->numRows)--;
    } else {
        int i = 0;
        while( i < rowNum){
            prevRow = rp;
            rp = rp->nextRow;
            i++;
        }
        tempRow = rp;
        prevRow->nextRow = tempRow->nextRow;
        free(tempRow);
        ss->firstRow = fr;
        (ss->numRows)--;    
    }
    return;
}

// Sorts the rows of the spreadsheet into ascending order, based on
// the strings in the specified column
extern void SS_Sort(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_Sort(--,%d)\n", colNum);
    int cmpfn1(const void *a, const void *b) {
	struct OneRow *aptr = *(struct OneRow **)a;
	struct OneRow *bptr = *(struct OneRow **)b;
        return strcmp(aptr->row[colNum], bptr->row[colNum]);
    }
    //copy item pointers into array
    struct OneRow *rp = ss->firstRow;
    int numItems=0;
    struct OneRow *p;
    int arraySize = 4;
    struct OneRow **arrayVersion;
    arrayVersion = calloc(arraySize, sizeof(struct OneRow *));
    numItems=0;
    for(p=rp; p!=NULL; p=p->nextRow) {
    	if (numItems >= arraySize) {
    		arraySize *= 2;
    		arrayVersion = realloc(arrayVersion, arraySize*sizeof(struct OneRow *));
    	}
    	arrayVersion[numItems++] = p;
    }
    qsort(arrayVersion, numItems, sizeof(struct OneRow *), cmpfn1);
    struct OneRow *nextRow = NULL;
    while(--numItems >= 0) {
    	arrayVersion[numItems]->nextRow = nextRow;
    	nextRow = arrayVersion[numItems];
    }
    free(arrayVersion);
    ss->firstRow = nextRow;
}

// Sorts the rows of the spreadsheet into ascending order, based on
// the values of the floating point numbers in the specified column
extern void SS_SortNumeric(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_SortNumeric(--,%d)\n", colNum);
    int cmpfn2(const void *a, const void *b) {
	struct OneRow *aptr = *(struct OneRow **)a;
	struct OneRow *bptr = *(struct OneRow **)b;
	if (atoi(aptr->row[colNum]) < atoi(bptr->row[colNum]))
		return -1;
	else if (atoi(aptr->row[colNum]) > atoi(bptr->row[colNum]))
		return +1;
	else
		return 0;
    }
    //copy item pointers into array
    struct OneRow *rp = ss->firstRow;
    int numItems=0;
    struct OneRow *p;
    int arraySize = 4;
    struct OneRow **arrayVersion;
    arrayVersion = calloc(arraySize, sizeof(struct OneRow *));
    numItems=0;
    for(p=rp; p!=NULL; p=p->nextRow) {
    	if (numItems >= arraySize) {
    		arraySize *= 2;
    		arrayVersion = realloc(arrayVersion, arraySize*sizeof(struct OneRow *));
    	}
    	arrayVersion[numItems++] = p;
    }
    qsort(arrayVersion, numItems, sizeof(struct OneRow *), cmpfn2);
    struct OneRow *nextRow = NULL;
    while(--numItems >= 0) {
    	arrayVersion[numItems]->nextRow = nextRow;
    	nextRow = arrayVersion[numItems];
    }
    free(arrayVersion);
    ss->firstRow = nextRow;
}
// Searches down the specified column for a row which contains text.
// The search starts at row number startNum;
// The result is the row number (where the first row is numbered 0).
// If the text is not found, the result is -1.
int SS_FindRow(SPREADSHEET *ss, int colNum, char *text, int startNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_FindRow(--,%d,%s,%d)\n",
            colNum, text, startNum);
    
    if (startNum >= ss->numRows || startNum < 0) {
        printf("Row number (%d) is out of range\n", startNum);
        return -1;
    } else if (colNum >= ss->numCols || colNum < 0) {
        printf("Column number (%d) is out of range\n", colNum);
        return -1;
    }
    struct OneRow *rp = ss->firstRow;
    int rwCount = 0;
    //Assume searching from low row num to high row num.
    //Position the find to the startRow row
       while(rwCount < (ss->numRows) && rp != NULL) {
             if ((rwCount >= startNum) && (rwCount < ss->numRows) ) {
                 if (strcmp(text, rp->row[colNum]) == 0)
                         return rwCount;
                 }
                 rp = rp->nextRow;
                 rwCount++;
        }
    return -1;
}

// Outputs the specified row of the spreadsheet.
// It is printed as one line on standard output.
void SS_PrintRow(SPREADSHEET *ss, int rowNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_PrintRow(--,%d)\n", rowNum);
    if (rowNum >= ss->numRows || rowNum < 0) {
        printf("Row number (%d) is out of range\n", rowNum);
        return;
    }
    struct OneRow *rp = ss->firstRow;
    while(rowNum > 0 && rp != NULL) {
        rp = rp->nextRow;
        rowNum--;
    }
    if (rp == NULL) {
        printf("Row number (%d) is out of range??\n", rowNum);
        return;        
    }
    int k;
    for( k = 0 ; k<ss->numCols; k++ ) {
        if (k>0)
            printf(", ");
        printf("%s", rp->row[k]);
    }
    putchar('\n');
    return;
}

// The specified column must contain the textual representations of numbers
// (either integer or floating point). The sum of the numbers in the column
// is returned as a floating point number.
double SS_ColumnSum(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_ColumnSum(--,%d)\n", colNum);
    double sum = 0.0;
    if (colNum >= ss->numCols || colNum < 0) {
        printf("Column number (%d) is out of range\n", colNum);
        return sum;
    }
    struct OneRow *rp = ss->firstRow;
    int k;
    //Add column values from row 0 to (ss->numRows) -1.
        for(k = 0; k < (ss->numRows) && rp != NULL; k++) {
                sum += atof(rp->row[colNum]);
                rp = rp->nextRow;
        }
                              
    return sum;
}

double SS_ColumnAvg(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_ColumnAvg(--,%d)\n", colNum);
    double sum = SS_ColumnSum(ss,colNum);
    return sum/ss->numRows;
}


// The strdup function is provided in many but not all variants of the
// C library. Here it is, renamed as mystrdup, just in case.
char *mystrdup(char *s) {
	int len = strlen(s);
	char *result = malloc(len+1);
	CHECKMALLOC(result);
	return strcpy(result, s);
}
