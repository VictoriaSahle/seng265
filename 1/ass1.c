/* File: ass1.c

   DO NOT CHANGE THIS FILE!

   Driver program for SEng265 Assignment 1

   Author: NH
   Date:   14 Sept 2014

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "csvfunctions.h"

char *programName;
static char *delims = " \t\n\r\f";

// function headers to allow forward use
static void findrow(SPREADSHEET *ss);
static void printrow(SPREADSHEET *ss);
static void evalsum(SPREADSHEET *ss);
static void evalavg(SPREADSHEET *ss);
static void help();
static int convertColId( char *text );

// Main function where execution begins
//
// Usage:
//     csvprog [-d] filename.csv
// where the -d flag enables debugging output
// Once started, the program repeatedly prompts for
// subcommands to be entered.
int main( int argc, char *argv[] ) {
    int i;
    char *fileName = NULL;
    int debug = 0;

    programName = argv[0];
    for( i=1; i<argc; i++ ) {
        char *arg = argv[i];
        if (arg[0] == '-') {
            if (strcmp(arg,"-d") == 0) {
                debug = 1;
            } else {
                fprintf(stderr, "%s: unknown option %s\n", programName, arg);
            }
        } else {
            // it must be a CVS file
            if (fileName == NULL)
                fileName = arg;
            else
                fprintf(stderr, "%s: only one input file allowed\n", programName);
        }
    }
    if (fileName == NULL) {
        fprintf(stderr, "%s: no input file specified\n", programName);
        return 1;  // non-zero indicates a problem
    }

    SPREADSHEET *ss = SS_ReadCSV(fileName);
    if (ss == NULL) {
        fprintf(stderr, "%s: spreadsheet failed to load, exiting\n", programName);
        return 1;
    }

    SS_SetDebug(debug);

    char lineBuffer[80];
    char *command;

    // now process queries from standard input
    for( ; ; ) {
        printf("\nEnter a subcommand ==> ");
        fflush(stdout);

        // read one line
        if (fgets(lineBuffer, 80, stdin) == NULL)
            break;

        command = strtok(lineBuffer, delims);
        if (command == NULL || strcmp(command,"quit") == 0)
            break;
        if (strcmp(command,"findrow") == 0)
            findrow(ss);
        else
        if (strcmp(command,"printrow") == 0)
            printrow(ss);
        else
        if (strcmp(command,"evalsum") == 0)
            evalsum(ss);
        else
        if (strcmp(command,"evalavg") == 0)
            evalavg(ss);
        else
        if (strcmp(command,"help") == 0)
            help();
        else
            printf("-- unrecognized command (%s), type help for a list of commands\n", command);
    }
    return 0;
}

// Finds and outputs the number of the row which contains the
// specified text in the specified column, optionally starting at
// a specified row number
static void findrow(SPREADSHEET *ss) {
    char *col, *text, *startPos;
    int colNum = 0;
    int startNum = 0;
    col = strtok(NULL, delims);
    if (col == NULL) {
        printf("missing column specification\n");
        return;
    }
    text = strtok(NULL, delims);
    if (text == NULL) {
        printf("missing search text\n");
        return;
    }
    startPos = strtok(NULL, delims);
    if (startPos != NULL) {
        startNum = atoi(startPos);
    }
    colNum = convertColId(col);
    if (colNum < 0)
        return;
    int r = SS_FindRow(ss, colNum, text, startNum);
    if (r >= 0)
        printf("-- found in row %d\n", r);
    else
        printf("-- no matching row was found\n");
}

// Prints the row specified by its row number
static void printrow(SPREADSHEET *ss) {
    char *startRow, *endRow;
    int rowNum = 0;
    int endRowNum = 0;
    startRow = strtok(NULL, delims);
    if (startRow == NULL) {
        printf("missing row number\n");
        return;
    }
    rowNum = atoi(startRow);
    endRow = strtok(NULL, delims);
    if (endRow != NULL)
        endRowNum = atoi(endRow);
    do {
        SS_PrintRow(ss, rowNum);
    } while(++rowNum <= endRowNum);
}

static void evalsum(SPREADSHEET *ss) {
    char *col;
    int colNum = 0;
    col = strtok(NULL, delims);
    if (col == NULL) {
        printf("missing column specification\n");
        return;
    }
    colNum = convertColId(col);
    if (colNum < 0)
        return;
    double sum = SS_ColumnSum(ss, colNum);
    printf("-- sum = %f\n", sum);
}

static void evalavg(SPREADSHEET *ss) {
    char *col;
    int colNum = 0;
    col = strtok(NULL, delims);
    if (col == NULL) {
        printf("missing column specification\n");
        return;
    }
    colNum = convertColId(col);
    if (colNum < 0)
        return;
    double avg = SS_ColumnAvg(ss, colNum);
    printf("-- average = %f\n", avg);
}

static void help() {
    char *helptext[] = {
        "\nThe valid commands are:",
        "quit                     -- to exit the program",
        "help                     -- to display this help message",
        "findrow <col> <text>     -- print the number of the first row",
        "                            such that column <col> holds <text>",
        "findrow <col> <text> <n> -- similarly, except search starts at",
        "                            row number n",
        "printrow <n>             -- print row number <n>",
        "evalsum <col>            -- print the sum of the numbers that",
        "                            are in column <col>",
        "evalavg <col>            -- print the average of the numbers that",
        "                            are in column <col>",
        NULL
        };
    char **line;
    for(line = helptext; *line != NULL; line++) {
        printf("%s\n", *line);
    }
    putchar('\n');
}

// Columns can be identified by number (e.g. 17) or by the letter code
// used in Excel. Only single letters are supported, but numbers can
// be as large as desired.
//    convertColId("A"), convertColId("0") return 0
//    ...
//    convertColId("Z"), convertColId("25") return 25
static int convertColId( char *text ) {
    char c;
    int len = strlen(text);
    c = text[0];
    if (c >= 'A' && c <= 'Z' && len == 1) {
        return c-'A';
    }
    if (c >= '0' && c <= '9') {
        return atoi(text);
    }
    printf("-- bad column specification");
    return -1;
}

