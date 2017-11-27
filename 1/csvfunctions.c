/*
********************************************
*** ID                  : xxxxxxxxx
*** Name                : Victoria Sahle
*** Date                : September 24, 2014
*** Program Name        : csvfunctions.c
********************************************
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvfunctions.h"

#define MAXINPUTLINELEN     256

static SPREADSHEET theSS;   // the spreadsheet to work on
static int debug = 0;
static int tooWide = 0;


// forward declaration
static int extractItems(char *line, char row[][MAXLEN]);

void SS_SetDebug(int dbg) {
    debug = dbg;
}

SPREADSHEET *SS_ReadCSV(char *fileName) {
    char line[MAXINPUTLINELEN];
    SPREADSHEET result;
    int i;

    result.fileName = fileName;
    result.numRows = result.numCols = 0;

    FILE *f = fopen(fileName, "r");
    if (f == NULL) {
        fprintf(stderr, "Unable to read from file %s\n", fileName);
        perror(fileName);
        return NULL;
    }
    for( i = 0; i < MAXROWS; i++) {
        if (fgets(line, MAXINPUTLINELEN, f) == NULL) {
            // i--;    <=== this was a mistake!!
            break;
        }
        int k = extractItems(line, result.contents[i]);
        if (result.numCols == 0) {
            result.numCols = k;
        } else
        if (result.numCols != k) {
            fprintf(stderr, "Row %d has different number of columns from first row\n", i);
        }
    }
    result.numRows = i;
    fclose(f);
    memcpy(&theSS, &result, sizeof(SPREADSHEET));
    if (tooWide)
        fprintf(stderr, "Warning, number of columns exceeded the max of %d\n", MAXCOLS);
    return &theSS;
}

static int extractItems(char *line, char row[][MAXLEN]) {
    char *item;
    int col = 0;
    char items[MAXLEN];
	int i;
	int q1 = 0;
	int quoteCount = 0;
	for (i = 0; i < strlen(line); i++){
		if (line[i] == '"'){
	    	quoteCount++;
	        if (quoteCount == 3) {
	        	items[q1] = line[i];
	            q1++;
	            quoteCount = 1;
	        }
	    } else if (line[i] == ',') {
	    	if (quoteCount == 1){
	        	items[q1] = line[i];
	            q1++;
	        } else if (quoteCount != 1) {
	        	quoteCount = 0;
	            item = items;
	            if (col >= MAXCOLS) {
	            	tooWide = 1;
	            	break;
				}
				strncpy(row[col], item, q1);
				row[col][q1] = '\0'; //force null termination
				col++;
				q1 = 0;
	            int j;
	            for (j = 0; j < MAXLEN; j++)
	            	items[j] = '\0';
	        }
	    } else {
	            items[q1] = line[i];
	            q1++;
	    }
	}
	//Get the last item
	if (col >= MAXCOLS)
		tooWide = 1;
	else {
		item = items;
		strncpy(row[col], item, q1);
		row[col][q1] = '\0'; //force null termination
		col++;
	}
    return col;
}


// Searches down the specified column for a row which contains text.
// The search starts at row number rowNum;
// The result is the row number (where the first row is numbered 0).
// If the text is not found, the result is -1.
int SS_FindRow(SPREADSHEET *ss, int colNum, char *text, int startNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_FindRow(--,%d,%s,%d)\n",
            colNum, text, startNum);
	if(startNum > ss->numRows-1 && colNum > ss->numCols-1){
		printf("Row and Column number are out of range.");
		return -1;
	}
	else if(colNum > ss->numCols-1){
		printf("Column number is out of range.");
		return -1;
	}else if(startNum > ss->numCols-1){
		printf("Row number is out of range.");
		return -1;
	}
	
	int x;
	for(x = startNum; x < ss->numRows; x++){
		if(strcmp(text, ss->contents[x][colNum]) == 0){
			return x;
		}
	}
    return -1;
}

// Outputs the specified row of the spreadsheet.
// It is printed as one line on standard output.
void SS_PrintRow(SPREADSHEET *ss, int rowNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_PrintRow(--,%d)\n", rowNum);
	if(rowNum > ss->numRows-1){
		printf("Row number %d is out of range.\n", rowNum);
		return;
	}
	int x;
	for(x = 0; x < ss->numCols; x++){
		printf("%s", ss->contents[rowNum][x]);
		if((x+1) < ss->numCols)
			printf(",");
	}
	printf("\n");
	    return;
}

// The specified column must contain the textual representations of numbers
// (either integer or floating point). The sum of the numbers in the column
// is returned as a floating point number.
double SS_ColumnSum(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_ColumnSum(--,%d)\n", colNum);
	
	if(colNum > ss->numCols-1){
		printf("Column number %d is out of range.\n", colNum);
		return 0.0;
	}
    // Note: atof(s) converts a string s to a float value
	int x;
	double y;
	y = 0.0;
	for(x = 0; x < ss->numRows; x++){
		y += atof(ss->contents[x][colNum]);
	}
    return y;
}
//The average of the numbers in the column is returned as a floating point number.
double SS_ColumnAvg(SPREADSHEET *ss, int colNum) {
    if (debug)
        fprintf(stderr, "DEBUG: Call to SS_ColumnAvg(--,%d)\n", colNum);
	
	if(colNum > ss->numCols-1){
		printf("Column number %d is out of range.\n", colNum);
		return 0.0;
	}
	double x = SS_ColumnSum(ss, colNum);
	x = x / ss->numRows;
    return x;
}

