#!/opt/bin/python3
""" 
********************************************
*** ID                  : xxxxxxxxx
*** Name                : Victoria Sahle
*** Date                : October 26, 2014
*** Program Name        : ass3.py
********************************************
"""

import csv
import sys
import operator

csvfile = open('testfile.csv')
csvreader = csv.reader(csvfile, delimiter=',')
list_of_rows = []
for row in csvreader:
	list_of_rows.append(row)

csvfile.close()

csvfile = open('testfile.csv', 'w')
csvwriter = csv.writer(csvfile, delimiter=',')
for row in list_of_rows:
	csvwriter.writerow(row)
csvfile.close()

print(list_of_rows)

numRows = len(list_of_rows)
numCols = len(list_of_rows[0])

spreadSheet = ('testfile.csv', numRows, numCols, list_of_rows)

#TASK 1 (REIMPLEMENT EXISTING COMMANDS)


#TASK 2 STATS
def SS_Stats(spreadSheet):
	print "File: ", spreadSheet[0]
	print "Rows: ", spreadSheet[1]
	print "Columns: ", spreadSheet[2]
#END STATS

#Sort 
#Change so sorts rows from any column users specifies
def Sort():
	new_list = sorted(list_of_rows, key=operator.itemgetter(0))
	list_of_rows = new_list
#End Sort

#Sort Numeric
def Sort_numeric():
	new_list = sorted(list_of_rows, key=operator.itemgetter(1))
	list_of_rows = new_list
	print(list_of_rows)
#End Sort Numeric

#Unload and Delete Row
def Unload():
	del list_of_rows[:]
def Delete_row():
	del list_of_rows[0:1]

#End Unload and Delete Row

#Merge

csvfile_two = open('testfile2.csv')
csvreader = csv.reader(csvfile_two, delimiter=',')
list_of_rows_two = []
for row in csvreader:
        list_of_rows_two.append(row)

csvfile_two.close()

csvfile_two = open('testfile2.csv', 'w')
csvwriter = csv.writer(csvfile_two, delimiter=',')
for row in list_of_rows_two:
        csvwriter.writerow(row)
csvfile_two.close()

print(list_of_rows_two)

numRows_two = len(list_of_rows)
numCols_two = len(list_of_rows[0])

spreadSheet_two = ('testfile2.csv', numRows_two, numCols_two, list_of_rows_two)



