#!/opt/usr/bin/python3
# Import modules
""" 
********************************************
*** ID                  : xxxxxxxxx
*** Name                : Victoria Sahle
*** Date                : November 26, 2014
*** Program Name        : ass4.py
********************************************
"""

import sys
import os.path
import os
import re

# The various extentions

folderpath = sys.argv[1]

os.chdir(folderpath)

c_ext = ['.c']
cpp_ext = ['.C','.cc','.cpp']

cSourceList = []
cppSourceList = []

def main():
   """Entry point."""
   print("Program started ... searching for source in " + folderpath)
   print()

   #
   # Create list of C source files.
   #

   cSourceList = buildclist()

   #
   # Create list of C++ source files.
   #

   cppSourceList = buildcpplist()

   #
   # If either of the two list is not empty,
   # process the list to create the Makefile
   #

   c_src_count = len(cSourceList)
   cpp_src_count = len(cppSourceList)
   total_src = c_src_count + cpp_src_count
   if c_src_count > 0 or cpp_src_count > 0: 
      fp = open('Makefile', 'w')
      if c_src_count > 0 and cpp_src_count > 0: 
         write_srcs_objs_prog(fp, cSourceList + cppSourceList)
      elif c_src_count > 0:
         write_srcs_objs_prog(fp, cSourceList)
      elif cpp_src_count > 0:
         write_srcs_objs_prog(fp, cppSourceList)

      #Now write make command for the individual source file
      if c_src_count > 0:
         writecmakecommands(fp, cSourceList, 'C')
      if cpp_src_count > 0:
         writecmakecommands(fp, cppSourceList, 'CPP')

      #Write the last Make directive
      fp.write('\n')
      fp.write('clean:\n')
      fp.write('\trm -f $(OBJS)\n\n')

      #Close the file before exiting.
      fp.close()
      print(repr(total_src) + " source files were found. Makefile has been created in " + folderpath)
   else: 
      print("No C or C++ souces were found, hence no Makefile was created.")
   print()
   print("Process completed successfuly.")

def buildclist():
   """Create list of C files."""
   cList = [f for f in os.listdir() if os.path.isfile(f)]
   cList = [f for f in cList
               if os.path.splitext(f)[1] in c_ext]
   return cList

def buildcpplist():
   """Create list of C++ files."""
   cppList = [f for f in os.listdir() if os.path.isfile(f)]
   cppList = [f for f in cppList
               if os.path.splitext(f)[1] in cpp_ext]
   return cppList

def writecmakecommands(fp, srcList, srcType):
   """Create make entiries for C and C++ files."""
   h_dict = {}
   dependency_list = []
   for f in srcList:
      dependency_list.append(f)
      h_dict = build_dictionary(f)
      numDictItem = len(h_dict)
      if numDictItem > 0:
         dependency_list.extend(list(h_dict.keys()))
      write_each_objs(fp, dependency_list, srcType)
      dependency_list.clear()
   return

def build_dictionary(filename):
   """Create a dictionary of .h file for this .c file."""
   master_dict = {}
   h_dict = {}
   h_dict = readfiles(filename)
   while 1:
      h_dict_len = len(h_dict)
      if h_dict_len > 0:
         for x in h_dict:
            if x not in master_dict:
               if h_dict[x] == 'X':
                  master_dict[filename+': contains #include for missing file '+x] = 'X' 
               else:
                  master_dict[x] = h_dict[x] 
            else:
               if h_dict[x] == 'X':
                  master_dict[filename+': contains #include for missing file '+x] = 'X' 
                  del master_dict[x]
      #Process master list again
      m_dict_len = len(master_dict)
      if m_dict_len > 0:
         item_found = 'N'
         file_to_search = ""
         for m_key in master_dict:
            if master_dict[m_key] == 'N':
               item_found = 'Y'
               master_dict[m_key] = 'Y'
               file_to_search = m_key
               break
      if item_found == 'Y':
         h_dict.clear()
         h_dict = readfiles(file_to_search)
      else:
         break
   return master_dict

def readfiles(filename):
   """Read file and look for .h includes."""
   try:
      the_dict = {}
      pstream = open(filename)
      for line in pstream:
         m = re.match("#include[ \t][\"][a-zA-Z0-9_\.h\"]", line)
         if m:
            h_file = line.split()
            the_dict[h_file[1].strip('"')] = 'N'
      pstream.close()
   except IOError as errno:
      the_dict[filename] = 'X'
   except:
      print("Unexpected error:", sys.exc_info()[0])
      raise
   return the_dict

def write_srcs_objs_prog(fp, allsrc):
   """Write SRCS, OBJS and PROG section of the Makefile."""
   allsrc.sort()
   fp.write('SRCS =')   
   for src in allsrc:
      fp.write(' ' + src)   
   fp.write('\n\n')   
   fp.write('OBJS =')   
   for src in allsrc:
      fp.write(' ' + src.split('.')[0] + '.o')
   fp.write('\n\n')   
   fp.write('PROG = prog.exe\n\n')   
   fp.write('$(PROG): $(OBJS)\n')   

   fp.write('\t$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(PROG)\n')   
   return

def write_each_objs(fp, thesrc, srcType):
   """Write the .o rule for each source to the Makefile."""
   fp.write('\n')   
   fp.write(thesrc[0].split('.')[0] + '.o:')
   for src in thesrc:
      fp.write(' ' + src)
   fp.write('\n')   
   if srcType == 'C':
      fp.write('\t$(CC) $(CPPFLAGS) $(CFLAGS) -c ' + thesrc[0] + '\n')   
   else:
      fp.write('\t$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ' + thesrc[0] + '\n')   

   return

if __name__ == "__main__":
   main()
